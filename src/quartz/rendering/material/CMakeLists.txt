#====================================================================
# The Rendering Material library
#====================================================================
add_library(
    QUARTZ_RENDERING_Material
    SHARED
    Material.hpp
    Material.cpp
)

target_include_directories(
    QUARTZ_RENDERING_Material
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    QUARTZ_RENDERING_Material
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    QUARTZ_RENDERING_Material
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_compile_definitions(
        QUARTZ_RENDERING_Material
        PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    QUARTZ_RENDERING_Material

    PUBLIC
    # some vendor lib

    PUBLIC
    MATH_Transform

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_RENDERING_Device
    QUARTZ_RENDERING_Texture
)