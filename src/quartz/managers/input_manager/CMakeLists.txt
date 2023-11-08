#====================================================================
# The Managers InputManager library
#====================================================================
add_library(
        QUARTZ_MANAGERS_InputManager
        SHARED
        InputManager.cpp
)

target_compile_options(
        QUARTZ_MANAGERS_InputManager
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_MANAGERS_InputManager

        PUBLIC
        glfw

        PUBLIC
        UTIL_Logger
)