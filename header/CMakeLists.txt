set(LIBRARY_NAME mmio-mpi)

file(GLOB SRCFILES src/*.c)
file(GLOB HFILES include/*.h)

add_library(${LIBRARY_NAME} ${SRCFILES} ${HFILES} )
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${LIBRARY_NAME} PRIVATE MPI::MPI_C)
