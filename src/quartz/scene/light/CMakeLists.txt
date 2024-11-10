#====================================================================
# The Scene Light library
#====================================================================
add_library(
    QUARTZ_SCENE_Light
    SHARED
    AmbientLight.hpp
    AmbientLight.cpp

    DirectionalLight.hpp
    DirectionalLight.cpp

    PointLight.hpp
    PointLight.cpp

    SpotLight.hpp
    SpotLight.cpp
)

target_include_directories(
    QUARTZ_SCENE_Light
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_SCENE_Light
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_SCENE_Light
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_SCENE_Light
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_SCENE_Light

    PUBLIC
    MATH_Transform
)