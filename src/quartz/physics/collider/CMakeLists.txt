#====================================================================
# The Physics Collider library
#====================================================================
add_library(
    QUARTZ_PHYSICS_Collider
    SHARED
    Collider.hpp
    Collider.cpp

    BoxShape.hpp
    BoxShape.cpp

    SphereShape.hpp
    SphereShape.cpp
)

target_include_directories(
    QUARTZ_PHYSICS_Collider
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_PHYSICS_Collider
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_PHYSICS_Collider
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_PHYSICS_Collider

    PUBLIC
    reactphysics3d

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger
)

