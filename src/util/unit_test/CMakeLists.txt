#====================================================================
# The unit test utility library
#====================================================================
add_library(
    UTIL_UnitTest
    SHARED
    UnitTest.hpp
    UnitTest.cpp
)

target_include_directories(
    UTIL_UnitTest
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_UnitTest
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_UnitTest
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    UTIL_UnitTest

    PUBLIC
    UTIL_Logger
)
