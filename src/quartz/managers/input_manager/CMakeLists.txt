#====================================================================
# The Managers InputManager library
#====================================================================
add_library(
    QUARTZ_MANAGERS_InputManager
    SHARED
    InputManager.hpp
    InputManager.cpp
)

target_include_directories(
    QUARTZ_MANAGERS_InputManager
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_MANAGERS_InputManager
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_MANAGERS_InputManager
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_MANAGERS_InputManager
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_MANAGERS_InputManager

    PUBLIC
    glfw

    PUBLIC
    UTIL_Logger
)