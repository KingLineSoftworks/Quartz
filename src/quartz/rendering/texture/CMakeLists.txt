#====================================================================
# The Rendering Texture library
#====================================================================
add_library(
    QUARTZ_RENDERING_Texture
    SHARED
    Texture.hpp
    Texture.cpp
)

target_include_directories(
    QUARTZ_RENDERING_Texture
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_RENDERING_Texture
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_RENDERING_Texture
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Texture
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
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