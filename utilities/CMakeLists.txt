
add_library(utilities INTERFACE)
target_include_directories(utilities INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(utilities INTERFACE fmt)
target_link_libraries(utilities INTERFACE Boost::program_options)
