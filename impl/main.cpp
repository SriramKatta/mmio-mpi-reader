#include "mmio-mpi.hpp"
#include "parseCLA.hpp"
#include <boost/mpi.hpp>
#include <fmt/format.h>
namespace mpi = boost::mpi;

int main(int argc, char *argv[]) {
  mpi::environment env(argc, argv);
  mpi::communicator world;
  std::string fname;
  bool writerankfiles;
  parseCLA(argc, argv, fname, writerankfiles);
#if 0
  read_file("../matrix/bcsstm04.mtx", true);
#else
  read_file(fname, writerankfiles);
#endif
  return 0;
}
