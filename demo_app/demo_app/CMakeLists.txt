#====================================================================
# The executable target application (the *game* oooooh)
#====================================================================
set(APPLICATION_NAME "Demo_App_The_Sequel")

include(ApplicationVersion)

add_executable(
    ${APPLICATION_NAME}
    main.cpp
    Boilerplate.hpp
    Boilerplate.cpp
    SceneParameters.hpp
    SceneParameters.cpp
    player/Player.hpp
    player/Player.cpp
)

target_include_directories(
    ${APPLICATION_NAME}
    PRIVATE
    ${QUARTZ_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_options(
    ${APPLICATION_NAME}
    PUBLIC
    ${QUARTZ_CMAKE_CXX_FLAGS}
)

# pre compile definitions for the target
target_compile_definitions(
    ${APPLICATION_NAME}
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
    PUBLIC APPLICATION_NAME="${APPLICATION_NAME}"
    PUBLIC APPLICATION_VERSION
    PUBLIC APPLICATION_MINOR_VERSION=${APPLICATION_MINOR_VERSION}
    PUBLIC APPLICATION_PATCH_VERSION=${APPLICATION_PATCH_VERSION}
    PUBLIC APPLICATION_MAJOR_VERSION=${APPLICATION_MAJOR_VERSION}
)

# Link everything to the application
target_link_libraries(
    ${APPLICATION_NAME}

    # Vendor (leaving this commented section in here for ease of placement in the future when prototyping)
    PRIVATE
    reactphysics3d

    # Math
    PRIVATE
    MATH_Transform

    # Utility
    PRIVATE
    UTIL_Logger

    # Quartz
    PRIVATE
    QUARTZ_Application
)

