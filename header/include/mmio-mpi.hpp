#ifndef MMIO_MPI_HPP
#define MMIO_MPI_HPP
#pragma once

#include <string>
#include <vector>
#include <mpi.h>

struct Entry {
    int row;
    int col;
    double val;
    Entry(int r = 0, int c = 0, double v = 0.0) : row(r), col(c), val(v) {}
    ~Entry(){}
  };
void printfilewithrank(std::vector<Entry> &, int);
MPI_Datatype create_entry_type();
std::vector<Entry> read_file(const std::string &);

#endif
