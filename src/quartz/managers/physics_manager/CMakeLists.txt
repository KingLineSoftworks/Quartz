#====================================================================
# The Managers PhysicsManager library
#====================================================================
add_library(
    QUARTZ_MANAGERS_PhysicsManager
    SHARED
    PhysicsManager.hpp
    PhysicsManager.cpp
)

target_include_directories(
    QUARTZ_MANAGERS_PhysicsManager
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_MANAGERS_PhysicsManager
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_MANAGERS_PhysicsManager
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_MANAGERS_PhysicsManager

    PUBLIC
    reactphysics3d

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger
)
