add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE utilities) 
target_link_libraries(${PROJECT_NAME} PRIVATE mmio-mpi) 
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt) 
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::serialization) 
target_link_libraries(${PROJECT_NAME} PRIVATE Boost::mpi) 