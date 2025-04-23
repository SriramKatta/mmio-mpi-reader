#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mmio-mpi.hpp"
#include "parseCLA.hpp"

#define REACHED printf("rank %d reached %d\n", rank, __LINE__);

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);

  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // char fname[256] = {argv[1]};
  // parseCLA(argc, argv, fname);
  REACHED;
  Entry *res = NULL;
  int res_size = 0;
  REACHED;

  if (rank == 0) {
    REACHED;
    res = read_file(argv[1],
                    &res_size); // Must allocate and return entries + size
    printfilewithrank(res, res_size, rank);
    REACHED;
    free(res); // Don't forget to free memory
  }
  REACHED;

  MPI_Finalize();
  return 0;
}
