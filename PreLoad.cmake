#====================================================================
# CMake information
# This file will be preprocessed before the main CMakeLists.txt,
# essentially adding everything here as command line arguments to
# the cmake command
#====================================================================

set(CMAKE_GENERATOR "Unix Makefiles" CACHE INTERNAL "" FORCE)

set(CMAKE_C_COMPILER "clang" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER "clang++" CACHE STRING "" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "" FORCE)
