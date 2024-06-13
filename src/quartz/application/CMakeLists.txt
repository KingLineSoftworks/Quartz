#====================================================================
# The Application library
#====================================================================
add_library(
        QUARTZ_Application
        SHARED
        Application.hpp
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

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_MANAGERS_InputManager
        QUARTZ_RENDERING_Context
        QUARTZ_SCENE_Scene
)