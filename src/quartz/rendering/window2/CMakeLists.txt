#====================================================================
# The Window library
#====================================================================
add_library(
        QUARTZ_RENDERING_Window2
        SHARED
        Window2.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Window2
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Window2

        PUBLIC
        glfw
        vulkan

        PUBLIC
        UTIL_Logger
)