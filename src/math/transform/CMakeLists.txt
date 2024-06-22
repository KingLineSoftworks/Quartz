#====================================================================
# The math transform library
#====================================================================
add_library(
    MATH_Transform
    SHARED
    Vec2.hpp
    Vec2.cpp
    Vec3.hpp
    Vec3.cpp
    Vec4.hpp
    Vec4.cpp

    Mat4.hpp
    Mat4.cpp

    Quaternion.hpp
    Quaternion.cpp
)

target_include_directories(
    MATH_Transform
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    MATH_Transform
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    MATH_Transform
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)

target_link_libraries(
    MATH_Transform

    PUBLIC
    glm
    reactphysics3d

    PUBLIC
    MATH_Algorithms

    PUBLIC
    UTIL_Logger
)