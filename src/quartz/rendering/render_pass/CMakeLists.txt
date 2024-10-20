#====================================================================
# The Rendering Render Pass library
#====================================================================
add_library(
        QUARTZ_RENDERING_RenderPass
        SHARED
        RenderPass.hpp
        RenderPass.cpp
)

target_compile_options(
        QUARTZ_RENDERING_RenderPass
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_RENDERING_RenderPass
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_RENDERING_RenderPass

        PUBLIC
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Window
)