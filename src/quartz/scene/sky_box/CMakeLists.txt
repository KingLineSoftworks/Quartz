#====================================================================
# The Scene SkyBox library
#====================================================================
add_library(
        QUARTZ_SCENE_SkyBox
        SHARED
        SkyBox.hpp
        SkyBox.cpp
)

target_compile_options(
        QUARTZ_SCENE_SkyBox
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
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