#ifndef MMIO_MPI_H
#define MMIO_MPI_H
#pragma once

#include <mpi.h>

typedef struct {
    int row;
    int col;
    double val;
} Entry;

// Print the contents of a chunk to a rank-specific file
void printfilewithrank(Entry *vec, int size, int rank);

// Create a custom MPI datatype for the Entry struct
MPI_Datatype create_entry_type();

// Read and distribute a Matrix Market file using MPI (only rank 0 gets full data)
Entry* read_file(const char *fname, int *total_size);

#endif // MMIO_MPI_H
