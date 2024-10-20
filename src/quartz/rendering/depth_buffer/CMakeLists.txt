#====================================================================
# The Rendering Depth Buffer library
#====================================================================
add_library(
        QUARTZ_RENDERING_DepthBuffer
        SHARED
        DepthBuffer.hpp
        DepthBuffer.cpp
)

target_compile_options(
        QUARTZ_RENDERING_DepthBuffer
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_RENDERING_DepthBuffer
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_RENDERING_DepthBuffer

        PUBLIC
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_VulkanUtil
)