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
target_link_libraries(
    UNITTEST_Util

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_MANAGERS_PhysicsManager
)

#====================================================================
# Functionality for easily creating unit tests
#====================================================================

function(create_unit_test unit_test_name unit_test_main_file unit_test_link_libraries)
    add_executable(${unit_test_name} ${unit_test_main_file})
    target_compile_definitions(${unit_test_name} PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
    target_compile_options(${unit_test_name} PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
    target_include_directories(${unit_test_name} PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
    target_link_libraries(${unit_test_name} PRIVATE ${unit_test_link_libraries} UNITTEST_Util)
    add_test(NAME ${unit_test_name} COMMAND ${unit_test_name})
endfunction()

#====================================================================
# Math unit tests
#====================================================================

set(MATH_UNIT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/math")

create_unit_test(
    test_math_transform_quaternion
    "${MATH_UNIT_TEST_DIR}/test_math_transform_quaternion.cpp"
    MATH_Transform
)

#====================================================================
# Quartz unit tests
#====================================================================

SET(QUARTZ_UNIT_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/quartz")

#-------------------------------------------------------------------
# Quartz Physics unit tests
#-------------------------------------------------------------------

set(QUARTZ_PHYSICS_UNIT_TEST_DIR "${QUARTZ_UNIT_TEST_DIR}/physics")

list(APPEND test_rigidbody_collider_pointers_link_libraries
    QUARTZ_PHYSICS_Collider
    QUARTZ_PHYSICS_Field
    QUARTZ_PHYSICS_RigidBody
)
create_unit_test(
    test_rigidbody_collider_pointers
    "${QUARTZ_PHYSICS_UNIT_TEST_DIR}/rigid_body/test_rigidbody_collider_pointers.cpp"
    "${test_rigidbody_collider_pointers_link_libraries}"
)

#-------------------------------------------------------------------
# Quartz Scene unit tests
#-------------------------------------------------------------------

set(QUARTZ_SCENE_UNIT_TEST_DIR "${QUARTZ_UNIT_TEST_DIR}/scene")

list(APPEND test_doodad_rigidbody_collider_transforms_link_libraries
    QUARTZ_PHYSICS_Collider
    QUARTZ_PHYSICS_RigidBody
    QUARTZ_SCENE_Doodad
)
create_unit_test(
    test_doodad_rigidbody_collider_transforms
    "${QUARTZ_SCENE_UNIT_TEST_DIR}/doodad/test_doodad_rigidbody_collider_transforms.cpp"
    "${test_doodad_rigidbody_collider_transforms_link_libraries}"
)
