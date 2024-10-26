#====================================================================
# The Rendering Swapchain library
#====================================================================
add_library(
        QUARTZ_RENDERING_Swapchain
        SHARED
        Swapchain.hpp
        Swapchain.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Swapchain
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Swapchain
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_RENDERING_Swapchain

        PUBLIC
        glm
        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_DepthBuffer
        QUARTZ_RENDERING_Model
        QUARTZ_RENDERING_Pipeline
        QUARTZ_RENDERING_Window
        QUARTZ_RENDERING_VulkanUtil
        QUARTZ_SCENE_Doodad
)