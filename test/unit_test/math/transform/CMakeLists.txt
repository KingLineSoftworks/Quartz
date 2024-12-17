#====================================================================
# The Math Transform unit tests
#====================================================================

create_unit_test(
    test_math_transform_normalize
    test_math_transform_normalize.cpp
    MATH_Transform
)
create_unit_test(
    test_math_transform_quaternion
    test_math_transform_quaternion.cpp
    MATH_Transform
)
create_unit_test(
    test_math_transform_vec3
    test_math_transform_vec3.cpp
    MATH_Transform
)
