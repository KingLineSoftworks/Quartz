#====================================================================
# The Rendering Buffer library
#====================================================================
add_library(
        QUARTZ_RENDERING_Buffer
        SHARED
        BufferUtil.hpp
        BufferUtil.cpp

        ImageBuffer.hpp
        ImageBuffer.cpp

        LocallyMappedBuffer.hpp
        LocallyMappedBuffer.cpp

        StagedBuffer.hpp
        StagedBuffer.cpp

        StagedImageBuffer.hpp
        StagedImageBuffer.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Buffer
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Buffer

        PUBLIC
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_VulkanUtil
)