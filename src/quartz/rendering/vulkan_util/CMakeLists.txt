#====================================================================
# The Rendering Vulkan Utility library
#====================================================================
add_library(
        QUARTZ_RENDERING_VulkanUtil
        SHARED
        VulkanUtil.hpp
        VulkanUtil.cpp
)

target_compile_options(
        QUARTZ_RENDERING_VulkanUtil
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_VulkanUtil

        PUBLIC
        vulkan

        PUBLIC
        UTIL_Logger
)