cmake_minimum_required(VERSION 3.23 FATAL_ERROR)

project(mmio-mpi-reader LANGUAGES CXX C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executables)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(MPI REQUIRED)

add_subdirectory(header)
add_subdirectory(example)
