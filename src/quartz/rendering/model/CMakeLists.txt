#====================================================================
# The Rendering Model library
#====================================================================
add_library(
        QUARTZ_RENDERING_Model
        SHARED
        Vertex.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Model
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Model

        PUBLIC
        glm
        vulkan

        PUBLIC
        UTIL_Logger
)