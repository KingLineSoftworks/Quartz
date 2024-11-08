#====================================================================
# The Managers SceneManager library
#====================================================================
add_library(
    QUARTZ_MANAGERS_SceneManager
    SHARED
    SceneManager.hpp
    SceneManager.cpp
)

target_include_directories(
    QUARTZ_MANAGERS_SceneManager
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_MANAGERS_SceneManager
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_MANAGERS_SceneManager
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_MANAGERS_SceneManager

    PUBLIC
    # some external library

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_SCENE_Scene
)
