#====================================================================
# The Scene SkyBox library
#====================================================================
add_library(
    QUARTZ_SCENE_SkyBox
    SHARED
    SkyBox.hpp
    SkyBox.cpp
)

target_include_directories(
    QUARTZ_SCENE_SkyBox
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)


target_compile_options(
    QUARTZ_SCENE_SkyBox
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_SCENE_SkyBox
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
    QUARTZ_SCENE_SkyBox
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_SCENE_SkyBox

    PUBLIC
    # some vendor lib

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_RENDERING_CubeMap
    QUARTZ_RENDERING_Device
)

