#====================================================================
# The Scene Light library
#====================================================================
add_library(
        QUARTZ_SCENE_Light
        SHARED
        AmbientLight.cpp
        DirectionalLight.cpp
)

target_compile_options(
        QUARTZ_SCENE_Light
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_SCENE_Light

        PUBLIC
        glm
)