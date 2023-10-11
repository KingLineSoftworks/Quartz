#====================================================================
# The Vertex library
#====================================================================
add_library(
        QUARTZ_RENDERING_Vertex
        SHARED
        Vertex.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Vertex
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Vertex

        PUBLIC
        glm
        vulkan

        PUBLIC
        UTIL_Logger
)