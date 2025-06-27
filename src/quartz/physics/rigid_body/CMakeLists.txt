#====================================================================
# The Physics RigidBody library
#====================================================================
add_library(
    QUARTZ_PHYSICS_RigidBody
    SHARED
    RigidBody.hpp
    RigidBody.cpp
)

target_include_directories(
    QUARTZ_PHYSICS_RigidBody
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_PHYSICS_RigidBody
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_PHYSICS_RigidBody
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_PHYSICS_RigidBody

    PUBLIC
    reactphysics3d

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_PHYSICS_Collider
)
