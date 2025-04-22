#include <boost/mpi.hpp>
#include <fmt/format.h>
#include <fstream>
#include <mpi.h>
#include <vector>

#include "mmio-mpi.hpp"

namespace mpi = boost::mpi;

struct Entry {
  int row;
  int col;
  double val;
  Entry(int r, int c, double v) : row(r), col(c), val(v) {}
};

void printfilewithrank(std::vector<Entry> &vec, int rank) {
  std::string filename("rank");
  filename += '1' + rank;
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

void read_file(const std::string &fname, bool writerankfiles) {
  MPI_Offset data_offset = 0;
  MPI_Offset filesize;
#if 0
  filesize = std::filesystem::file_size(filename);
#else
  MPI_File fh;
  MPI_File_open(MPI_COMM_WORLD, fname.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL,
                &fh);
  MPI_File_get_size(fh, &filesize);
  MPI_File_close(&fh);
#endif
  size_t nrows = 0, ncols = 0, nnz = 100;
  std::ifstream fin(fname);
  if (!fin.is_open()) {
    fmt::print("failed to open the file {}\n", fname);
    exit(0);
  }

  mpi::communicator world;

  auto start = MPI_Wtime();
  auto localchunk = paralleldataload(fin, nrows, ncols, nnz, world.rank(),
                                     world.size(), filesize, data_offset);
  auto end = MPI_Wtime();
  int localsize = localchunk.size();
  int globalsum = 0;
  mpi::reduce(world, localsize, globalsum, std::plus<int>(), 0);

  if (0 == world.rank()) {
    double et = end - start;
    fmt::print("time taken : {} | bandwidth : {}\n", et, filesize / et / 1e9);
    fmt::print("nnz == globalsum {}\n", nnz == globalsum);
  }
  if (writerankfiles) {
    printfilewithrank(localchunk, world.rank());
  }
}