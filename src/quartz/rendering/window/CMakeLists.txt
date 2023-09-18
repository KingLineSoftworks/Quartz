#====================================================================
# The Window library
#====================================================================
add_library(
        QUARTZ_RENDERING_Window
        SHARED
        Window.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Window
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Window

        PUBLIC
        glfw

        PUBLIC
        UTIL_Logger
)