#====================================================================
# The file system utility library
#====================================================================
add_library(
    UTIL_FileSystem
    SHARED
    FileSystem.hpp
    FileSystem.cpp
)

target_include_directories(
    UTIL_FileSystem
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_FileSystem
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_FileSystem
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    UTIL_FileSystem

    PUBLIC
    UTIL_Logger
)