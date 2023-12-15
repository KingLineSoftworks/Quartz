#====================================================================
# The Scene Doodad library
#====================================================================
add_library(
        QUARTZ_SCENE_Doodad
        SHARED
        Doodad.cpp
        Transform.cpp
)

target_compile_options(
        QUARTZ_SCENE_Doodad
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_SCENE_Doodad

        PUBLIC
        glm

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Model
)