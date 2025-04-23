#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmio-mpi.hpp"


void printfilewithrank(Entry *vec, int size, int rank) {
  char filename[64];
  sprintf(filename, "rank%d", rank);
  FILE *fout = fopen(filename, "w");
  if (!fout) {
    printf("failure to open the out file\n");
    return;
  }

  for (int i = 0; i < size; ++i) {
    fprintf(fout, "%d %d %lf\n", vec[i].row, vec[i].col, vec[i].val);
  }

  fclose(fout);
}

MPI_Datatype create_entry_type() {
  MPI_Datatype entry_type;
  int lengths[3] = {1, 1, 1};
  MPI_Aint displacements[3];
  MPI_Datatype types[3] = {MPI_INT, MPI_INT, MPI_DOUBLE};

  Entry dummy = {0, 0, 0.0};
  MPI_Aint base;
  MPI_Get_address(&dummy, &base);
  MPI_Get_address(&dummy.row, &displacements[0]);
  MPI_Get_address(&dummy.col, &displacements[1]);
  MPI_Get_address(&dummy.val, &displacements[2]);

  for (int i = 0; i < 3; ++i) {
    displacements[i] -= base;
  }

  MPI_Type_create_struct(3, lengths, displacements, types, &entry_type);
  MPI_Type_commit(&entry_type);

  return entry_type;
}

Entry *paralleldataload(FILE *fin, size_t *nrows, size_t *ncols, size_t *nnz,
                        int rank, int size, MPI_Offset filesize,
                        MPI_Offset *data_offset, int *out_count) {

  char line[256];
  *data_offset = 0;

  while (fgets(line, sizeof(line), fin)) {
    *data_offset += strlen(line);
    if (line[0] != '%')
      break;
  }

  sscanf(line, "%lu %lu %lu", nrows, ncols, nnz);

  int numlines = *nnz / size + ((*nnz % size) > rank ? 1 : 0);
  MPI_Offset chunksize = (filesize - *data_offset) / size;
  MPI_Offset currentoffset = *data_offset + chunksize * rank;

  fseek(fin, currentoffset, SEEK_SET);
  MPI_Offset endoffset = currentoffset + chunksize;

  long current_pos = ftell(fin);
  if (current_pos > 0) {
    fseek(fin, current_pos - 1, SEEK_SET);
    char prev = fgetc(fin);
    if (prev != '\n') {
      fgets(line, sizeof(line), fin);
    }
  }

  Entry *chunk = (Entry *)malloc(numlines * sizeof(Entry));
  int count = 0;

  while (ftell(fin) < endoffset && fgets(line, sizeof(line), fin)) {
    if (line[0] == '\0' || line[0] == '%')
      continue;

    int r, c;
    double v;
    if (sscanf(line, "%d %d %lf", &r, &c, &v) == 3) {
      chunk[count++] = (Entry){r - 1, c - 1, v};
    }
  }

  *out_count = count;
  return chunk;
}

Entry *read_file(const char *fname, int *final_size) {
  MPI_File fh;
  MPI_Offset filesize;
  MPI_File_open(MPI_COMM_WORLD, fname, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
  MPI_File_get_size(fh, &filesize);
  MPI_File_close(&fh);

  FILE *fin = fopen(fname, "r");
  if (!fin) {
    printf("failed to open the file %s\n", fname);
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Datatype entry_type = create_entry_type();

#if BENCHMARK
  double start = MPI_Wtime();
#endif

  size_t nrows = 0, ncols = 0, nnz = 100;
  MPI_Offset data_offset = 0;
  int local_count;
  Entry *localchunk = paralleldataload(fin, &nrows, &ncols, &nnz, rank, size,
                                       filesize, &data_offset, &local_count);

#if BENCHMARK
  double end = MPI_Wtime();
#endif

  int *recvcounts = NULL, *displs = NULL;
  int total_count = 0;
  if (rank == 0) {
    recvcounts = (int *)malloc(size * sizeof(int));
    displs = (int *)malloc(size * sizeof(int));
  }

  MPI_Gather(&local_count, 1, MPI_INT, recvcounts, 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  Entry *gathered_data = NULL;
  if (rank == 0) {
    displs[0] = 0;
    for (int i = 1; i < size; ++i) {
      displs[i] = displs[i - 1] + recvcounts[i - 1];
    }
    total_count = displs[size - 1] + recvcounts[size - 1];
    gathered_data = (Entry*)malloc(total_count * sizeof(Entry));
  }

  MPI_Gatherv(localchunk, local_count, entry_type, gathered_data, recvcounts,
              displs, entry_type, 0, MPI_COMM_WORLD);

  if (rank == 0) {
#if BENCHMARK
    double et = end - start;
    printf("time taken: %f | bandwidth: %f GB/s\n", et,
           (double)filesize / et / 1e9);
#endif
    printf("test passed: %s\n", nnz == total_count ? "true" : "false");
  }

#if DEBUG
  printfilewithrank(localchunk, local_count, rank);
#endif

  MPI_Type_free(&entry_type);
  fclose(fin);
  free(localchunk);
  if (rank == 0) {
    free(recvcounts);
    free(displs);
    *final_size = total_count;
    return gathered_data;
  }

  return NULL;
}
