#====================================================================
# The Physics Field library
#====================================================================
add_library(
    QUARTZ_PHYSICS_Field
    SHARED
    Field.hpp
    Field.cpp
)

target_include_directories(
    QUARTZ_PHYSICS_Field
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_PHYSICS_Field
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_PHYSICS_Field
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_PHYSICS_Field

    PUBLIC
    reactphysics3d

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_MANAGERS_PhysicsManager
    QUARTZ_PHYSICS_RigidBody
)
