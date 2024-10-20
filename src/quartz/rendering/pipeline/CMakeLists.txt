#====================================================================
# The Rendering Pipeline library
#====================================================================
add_library(
        QUARTZ_RENDERING_Pipeline
        SHARED
        Pipeline.hpp
        Pipeline.cpp

        UniformBufferInfo.hpp
        UniformBufferInfo.cpp

        UniformSamplerCubeInfo.hpp
        UniformSamplerCubeInfo.cpp

        UniformSamplerInfo.hpp
        UniformSamplerInfo.cpp

        UniformTextureArrayInfo.hpp
        UniformTextureArrayInfo.cpp

        PushConstantInfo.cpp
        PushConstantInfo.hpp
)

target_compile_options(
        QUARTZ_RENDERING_Pipeline
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Pipeline
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_RENDERING_Pipeline

        PUBLIC
        glm
        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Model
        QUARTZ_RENDERING_RenderPass
        QUARTZ_RENDERING_Texture
        QUARTZ_RENDERING_Window
        QUARTZ_SCENE_Camera
        QUARTZ_SCENE_Doodad
        QUARTZ_SCENE_Light
)

add_dependencies(
        QUARTZ_RENDERING_Pipeline

        QUARTZ_RENDERING_Shaders
)