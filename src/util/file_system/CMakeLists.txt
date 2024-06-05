#====================================================================
# The file system utility library
#====================================================================
add_library(
    UTIL_FileSystem
    SHARED
    FileSystem.hpp
    FileSystem.cpp
)

target_compile_options(
    UTIL_FileSystem
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
    UTIL_FileSystem

    PUBLIC
    UTIL_Logger
)