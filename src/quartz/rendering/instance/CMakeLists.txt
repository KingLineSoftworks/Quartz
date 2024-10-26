#====================================================================
# The Rendering Instance library
#====================================================================
add_library(
        QUARTZ_RENDERING_Instance
        SHARED
        Instance.hpp
        Instance.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Instance
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Instance
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
        QUARTZ_RENDERING_Instance

        PUBLIC
        glfw
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        # QUARTZ_Something
)