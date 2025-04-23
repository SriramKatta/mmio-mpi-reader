set(GIT_SHALLOW ON)
set(BOOST_ENABLE_CMAKE ON)
set(BOOST_SKIP_INSTALL_RULES ON) # Set `OFF` for installation
set(BUILD_SHARED_LIBS OFF)
set(BOOST_ENABLE_MPI OFF)
set(BOOST_INCLUDE_LIBRARIES "program_options")

CPMAddPackage(
  NAME Boost
  VERSION 1.87.0 # Versions less than 1.85.0 may need patches for installation targets.
  URL https://github.com/boostorg/boost/releases/download/boost-1.86.0/boost-1.86.0-cmake.tar.xz
  URL_HASH SHA256=2c5ec5edcdff47ff55e27ed9560b0a0b94b07bd07ed9928b476150e16b0efc57   
)
