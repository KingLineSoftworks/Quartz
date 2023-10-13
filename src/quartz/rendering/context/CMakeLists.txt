#====================================================================
# The Rendering Context library
#====================================================================
add_library(
        QUARTZ_RENDERING_Context
        SHARED
        Buffer.cpp
        Device.cpp
        Instance.cpp
        Pipeline.cpp
        Swapchain.cpp
        Window2.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Context
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Context

        PUBLIC
        glm
        vulkan
        glfw

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Model
)