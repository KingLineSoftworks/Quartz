#====================================================================
# The SourceLocation utility library
#====================================================================
add_library(
    UTIL_SourceLocation
    SHARED
    SourceLocation.hpp
    SourceLocation.cpp
)

target_include_directories(
    UTIL_SourceLocation
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_SourceLocation
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_SourceLocation
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

