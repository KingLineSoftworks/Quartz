#====================================================================
# The Application library
#====================================================================
add_library(
    QUARTZ_Application
    SHARED
    Application.cpp
)

target_compile_options(
        QUARTZ_Application
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_Application

        PUBLIC
        glfw
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Window
)