#====================================================================
# The Rendering Pipeline library
#====================================================================
add_library(
        QUARTZ_RENDERING_Pipeline
        SHARED
        Pipeline.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Pipeline
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
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
        QUARTZ_RENDERING_Mesh
        QUARTZ_RENDERING_Texture
        QUARTZ_RENDERING_Window
        QUARTZ_SCENE_Camera
)