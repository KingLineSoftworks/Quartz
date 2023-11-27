#====================================================================
# The Rendering Mesh library
#====================================================================
add_library(
        QUARTZ_RENDERING_Mesh
        SHARED
        Mesh.cpp
        Primitive.cpp
        Vertex.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Mesh
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Mesh

        PUBLIC
        glm
        tinygltf
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Device
)