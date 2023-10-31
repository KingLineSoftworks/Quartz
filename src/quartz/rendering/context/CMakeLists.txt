#====================================================================
# The Rendering Context library
#====================================================================
add_library(
        QUARTZ_RENDERING_Context
        SHARED
        Buffer.cpp
        Context.cpp
        Device.cpp
        Instance.cpp
        Mesh.cpp
        Model.cpp
        Pipeline.cpp
        Swapchain.cpp
        Texture.cpp
        Vertex.cpp
        Window.cpp
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
        tinyobjloader

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        # QUARTZ_Something
)