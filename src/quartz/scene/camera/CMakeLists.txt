#====================================================================
# The Scene Camera library
#====================================================================
add_library(
        QUARTZ_SCENE_Camera
        SHARED
        Camera.cpp
)

target_compile_options(
        QUARTZ_SCENE_Camera
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_SCENE_Camera

        PUBLIC
        glm

        PUBLIC
        UTIL_Logger
)