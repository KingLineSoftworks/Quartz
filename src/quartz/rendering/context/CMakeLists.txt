#====================================================================
# The Rendering Context library
#====================================================================
add_library(
        QUARTZ_RENDERING_Context
        SHARED
        Context.hpp
        Context.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Context
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Context

        PUBLIC
        glfw
        glm
        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Instance
        QUARTZ_RENDERING_Model
        QUARTZ_RENDERING_Pipeline
        QUARTZ_RENDERING_RenderPass
        QUARTZ_RENDERING_Swapchain
        QUARTZ_RENDERING_Texture
        QUARTZ_RENDERING_Window
        QUARTZ_SCENE_Camera
        QUARTZ_SCENE_Doodad
        QUARTZ_SCENE_Light
        QUARTZ_SCENE_Scene
)