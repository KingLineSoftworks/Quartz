#====================================================================
# The math algorithms library
#====================================================================
add_library(
    MATH_Algorithms
    SHARED
    Algorithms.hpp
    Algorithms.cpp
)

target_include_directories(
    MATH_Algorithms
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    MATH_Algorithms
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    MATH_Algorithms
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    MATH_Algorithms

    PUBLIC
    # some vendor lib

    PUBLIC
    # MATH_Something
)