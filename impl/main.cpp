#include <fmt/format.h>
#include <mpi.h>
#include <string>
#include <vector>

#include "mmio-mpi.hpp"
#include "parseCLA.hpp"

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  std::string fname;
  parseCLA(argc, argv, fname);
  int rank = 0;
  int size = 1;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  std::vector<Entry> res = read_file(fname);
  printfilewithrank(res, rank);

  MPI_Finalize();
  return 0;
}
