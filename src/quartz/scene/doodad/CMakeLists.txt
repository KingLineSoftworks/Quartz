#====================================================================
# The Scene Doodad library
#====================================================================
add_library(
        QUARTZ_SCENE_Doodad
        SHARED
        Doodad.hpp
        Doodad.cpp

        Transform.hpp
        Transform.cpp
)

target_compile_options(
        QUARTZ_SCENE_Doodad
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
        QUARTZ_SCENE_Doodad
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
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