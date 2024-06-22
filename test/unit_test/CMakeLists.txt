#====================================================================
# Unit tests library
#====================================================================

set(QUARTZ_UNIT_TEST_INCLUDE_DIRS ${QUARTZ_INCLUDE_DIRS} ${QUARTZ_TEST_DIR})

add_library(
    UNITTEST_Util
    Util.hpp
    Util.cpp
)
target_compile_definitions(UNITTEST_Util PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
target_compile_options(UNITTEST_Util PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
target_include_directories(UNITTEST_Util PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
target_link_libraries(UNITTEST_Util PUBLIC UTIL_Logger)

#====================================================================
# Math unit tests
#====================================================================

set(MATH_UNIT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/math")

add_executable(test_math_transform_quaternion "${MATH_UNIT_TEST_DIR}/test_math_transform_quaternion.cpp")
target_compile_definitions(test_math_transform_quaternion PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
target_compile_options(test_math_transform_quaternion PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
target_include_directories(test_math_transform_quaternion PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
target_link_libraries(test_math_transform_quaternion PRIVATE MATH_Transform)
add_test(NAME test_math_transform_quaternion COMMAND test_math_transform_quaternion)



