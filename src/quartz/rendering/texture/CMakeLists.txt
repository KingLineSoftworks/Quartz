#====================================================================
# The Rendering Texture library
#====================================================================
add_library(
        QUARTZ_RENDERING_Texture
        SHARED
        Texture.hpp
        Texture.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Texture
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Texture

        PUBLIC
        tinygltf
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_VulkanUtil
)