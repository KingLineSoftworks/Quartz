#====================================================================
# The Rendering Cube Map library
#====================================================================
add_library(
        QUARTZ_RENDERING_CubeMap
        SHARED
        CubeMap.hpp
        CubeMap.cpp
)

target_compile_options(
        QUARTZ_RENDERING_CubeMap
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_CubeMap

        PUBLIC
        glm
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Texture
)