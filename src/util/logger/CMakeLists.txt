#====================================================================
# The logger utility library
#====================================================================
add_library(
    UTIL_Logger
    SHARED
    Logger.hpp
    Logger.cpp
)

target_include_directories(
    UTIL_Logger
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_Logger
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_Logger
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    UTIL_Logger

    PUBLIC
    spdlog

    PUBLIC
    UTIL_Errors
)