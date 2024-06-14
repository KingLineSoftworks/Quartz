#====================================================================
# The Rendering Model library
#====================================================================
add_library(
    QUARTZ_RENDERING_Model
    SHARED
    Mesh.hpp
    Mesh.cpp

    Model.hpp
    Model.cpp

    Node.hpp
    Node.cpp

    Primitive.hpp
    Primitive.cpp

    Scene.hpp
    Scene.cpp

    TangentCalculator.hpp
    TangentCalculator.cpp

    Vertex.hpp
    Vertex.cpp
)

target_include_directories(
    QUARTZ_RENDERING_Model
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_RENDERING_Model
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_RENDERING_Model
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Model
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_RENDERING_Model

    PUBLIC
    glm
    mikktspace
    tinygltf
    vulkan

    PUBLIC
    UTIL_FileSystem
    UTIL_Logger

    PUBLIC
    QUARTZ_RENDERING_Buffer
    QUARTZ_RENDERING_Device
    QUARTZ_RENDERING_Material
    QUARTZ_RENDERING_Texture
)