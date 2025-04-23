#ifndef MMIO_MPI_HPP
#define MMIO_MPI_HPP
#pragma once

#include <string>
#include <vector>

struct Entry;
void printfilewithrank(std::vector<Entry> &, int);
std::vector<Entry> ead_file(const std::string &);

#endif
