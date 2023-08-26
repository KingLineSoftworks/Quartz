#====================================================================
# The logger library
#====================================================================
add_library(
    UTIL_Logger
    SHARED
    Logger.cpp
)

target_compile_options(
    UTIL_Logger
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
    UTIL_Logger
    PUBLIC
    spdlog
)