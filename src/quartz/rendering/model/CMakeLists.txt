#====================================================================
# The Rendering Model library
#====================================================================
add_library(
        QUARTZ_RENDERING_Model
        SHARED
        Mesh.cpp
        Model.cpp
        Node.cpp
        Primitive.cpp
        Scene.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Model
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Model

        PUBLIC
        glm
        tinygltf
        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Buffer
        QUARTZ_RENDERING_Material
        QUARTZ_RENDERING_Mesh
        QUARTZ_RENDERING_Texture
)