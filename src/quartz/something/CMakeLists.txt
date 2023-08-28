#====================================================================
# The Something library
#====================================================================
add_library(
    QUARTZ_Something
    SHARED
    Something.cpp
)

target_compile_options(
    QUARTZ_Something
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
    QUARTZ_Something
    PUBLIC
    UTIL_Logger
)