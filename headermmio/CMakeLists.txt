set(libname mmio)

add_library(${libname} src/mmio.c)
target_include_directories( ${libname} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)