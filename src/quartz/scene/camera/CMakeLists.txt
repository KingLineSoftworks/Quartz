#====================================================================
# The Scene Camera library
#====================================================================
add_library(
    QUARTZ_SCENE_Camera
    SHARED
    Camera.hpp
    Camera.cpp
)

target_include_directories(
    QUARTZ_SCENE_Camera
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)


target_compile_options(
    QUARTZ_SCENE_Camera
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_SCENE_Camera
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_SCENE_Camera
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_SCENE_Camera

    PUBLIC
    glm

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_MANAGERS_InputManager
)