#====================================================================
# The Scene Scene library
#====================================================================
add_library(
        QUARTZ_SCENE_Scene
        SHARED
        Scene.hpp
        Scene.cpp
)

target_compile_options(
        QUARTZ_SCENE_Scene
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_SCENE_Scene
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_SCENE_Scene

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_MANAGERS_InputManager
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Texture
        QUARTZ_RENDERING_Window
        QUARTZ_SCENE_Camera
        QUARTZ_SCENE_Doodad
        QUARTZ_SCENE_Light
        QUARTZ_SCENE_SkyBox
)