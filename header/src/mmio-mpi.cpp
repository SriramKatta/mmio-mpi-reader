#include <fmt/format.h>
#include <fstream>
#include <mpi.h>
#include <vector>

#include "mmio-mpi.hpp"

struct Entry {
  int row;
  int col;
  double val;
  Entry(int r, int c, double v) : row(r), col(c), val(v) {}
};

void printfilewithrank(std::vector<Entry> &vec, int rank) {
  std::string filename("rank");
  filename += std::to_string(rank);
  std::ofstream fout(filename);
  if (!fout.is_open()) {
    fmt::print("failure to open the out file\n");
  }

  for (auto &[row, col, val] : vec) {
    fout << row << " " << col << " " << val << std::endl;
  }
}

std::vector<Entry> paralleldataload(std::ifstream &fin, size_t &nrows,
                                    size_t &ncols, size_t &nnz, int rank,
                                    int size, int filesize, int data_offset) {

  std::string line;

  data_offset = 0;
  while (std::getline(fin, line)) {
    data_offset += line.length() + 1; // +1 for newline character
    if (!line.empty() && line[0] != '%')
      break;
  }
  sscanf(line.c_str(), "%lu %lu %lu", &nrows, &ncols, &nnz);

  int numlines = nnz / size + ((nnz % size) > rank ? 1 : 0);
  MPI_Offset chunksize = (filesize - data_offset) / size;
  MPI_Offset currentoffset = data_offset + chunksize * rank;
  fin.seekg(currentoffset);

  MPI_Offset endoffset = currentoffset + chunksize;
  std::vector<Entry> localchunk;
  localchunk.reserve(numlines);

  MPI_Offset current_pos = fin.tellg(); // get current position
  if (current_pos > 0) {
    fin.seekg(current_pos - 1);
    char prev;
    fin.get(prev);
    if (prev != '\n') {
      std::getline(fin, line);
    }
  }

  while (fin.tellg() < endoffset) {
    std::getline(fin, line);
    if (line.empty()) {
      break;
    }
    int r, c;
    double v;
    if (sscanf(line.c_str(), "%d %d %lf", &r, &c, &v) == 3) {
      localchunk.emplace_back(r - 1, c - 1, v);
    }
  }
  return localchunk;
}

std::vector<Entry> read_file(const std::string &fname, bool writerankfiles) {
  MPI_Offset data_offset = 0;
  MPI_Offset filesize;
  MPI_File fh;
  MPI_File_open(MPI_COMM_WORLD, fname.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL,
                &fh);
  MPI_File_get_size(fh, &filesize);
  MPI_File_close(&fh);
  size_t nrows = 0, ncols = 0, nnz = 100;
  std::ifstream fin(fname);
  if (!fin.is_open()) {
    fmt::print("failed to open the file {}\n", fname);
    exit(0);
  }
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

#if BENCHMARK
  auto start = MPI_Wtime();
#endif
  auto localchunk = paralleldataload(fin, nrows, ncols, nnz, rank, size,
                                     filesize, data_offset);
#if BENCHMARK
  auto end = MPI_Wtime();
#endif

  // Step 1: Gather sizes
  std::vector<int> recvcounts(size);
  int sendcount = localchunk.size();
  MPI_Gather(&sendcount, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0,
             MPI_COMM_WORLD);

  // Step 2: Compute displacements on rank 0
  std::vector<int> displs;
  int total_count = 0;
  if (rank == 0) {
    displs.resize(size);
    for (int i = 0; i < size; ++i) {
      displs[i] = total_count;
      total_count += recvcounts[i];
    }
  }

  std::vector<Entry> gathered_data;
  if (rank == 0) {
    gathered_data.resize(total_count);
  }

  MPI_Gatherv(localchunk.data(), sendcount, MPI_DOUBLE, gathered_data.data(),
              recvcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if (0 == rank) {
#if BENCHMARK
    double et = end - start;
    fmt::print("time taken : {} | bandwidth : {}\n", et, filesize / et / 1e9);
#endif
    fmt::print("test passed : {}\n", nnz == total_count);
  }
#if DEBUG
  printfilewithrank(localchunk, rank);
#endif

  return gathered_data;
}