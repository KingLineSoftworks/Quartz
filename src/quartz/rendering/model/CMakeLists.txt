#====================================================================
# The Rendering Model library
#====================================================================
add_library(
        QUARTZ_RENDERING_Model
        SHARED
        Model.cpp
        Node.cpp
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
        QUARTZ_RENDERING_Material
        QUARTZ_RENDERING_Mesh
        QUARTZ_RENDERING_Texture
)