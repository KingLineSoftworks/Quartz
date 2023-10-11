#====================================================================
# The Pipeline library
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
        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Vertex
        QUARTZ_RENDERING_Window2
)