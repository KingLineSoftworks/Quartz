#====================================================================
# The Scene Doodad library
#====================================================================
add_library(
    QUARTZ_SCENE_Doodad
    SHARED
    Doodad.hpp
    Doodad.cpp
)

target_include_directories(
    QUARTZ_SCENE_Doodad
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)


target_compile_options(
    QUARTZ_SCENE_Doodad
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_SCENE_Doodad
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
    QUARTZ_SCENE_Doodad
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_SCENE_Doodad

    PUBLIC
    glm

    PUBLIC
    MATH_Algorithms
    MATH_Transform

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_MANAGERS_PhysicsManager
    QUARTZ_PHYSICS_Realm
    QUARTZ_PHYSICS_RigidBody
    QUARTZ_RENDERING_Device
    QUARTZ_RENDERING_Model
)
