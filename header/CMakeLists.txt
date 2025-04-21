set(LIBRARY_NAME mmio-mpi)

file(GLOB SRCFILES src/*.cpp)
file(GLOB HFILES include/*.hpp)

add_library(${LIBRARY_NAME} ${SRCFILES} ${HFILES} )
target_include_directories(${LIBRARY_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${LIBRARY_NAME} PRIVATE fmt)
target_link_libraries(${LIBRARY_NAME} PRIVATE Boost::mpi)
target_link_libraries(${LIBRARY_NAME} PRIVATE MPI::MPI_CXX)
