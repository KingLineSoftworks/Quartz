#====================================================================
# The executable target application (the *game* oooooh)
#====================================================================
set(APPLICATION_NAME "Jolt_Demo_App")

include(ApplicationVersion)

add_executable(
    ${APPLICATION_NAME} ${JOLT_TESTING_SOURCE_DIR}/jolt_main.cpp
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

# Link everything to the application
target_link_libraries(
    ${APPLICATION_NAME}

    PRIVATE
    jolt

    # Utility
    PRIVATE
    UTIL_Logger

    # Quartz
    PRIVATE
    QUARTZ_Application
)

# pre compile definitions for the target
target_compile_definitions(
    ${APPLICATION_NAME}
    PUBLIC APPLICATION_NAME="${APPLICATION_NAME}"
    PUBLIC APPLICATION_VERSION
    PUBLIC APPLICATION_MINOR_VERSION=${APPLICATION_MINOR_VERSION}
    PUBLIC APPLICATION_PATCH_VERSION=${APPLICATION_PATCH_VERSION}
    PUBLIC APPLICATION_MAJOR_VERSION=${APPLICATION_MAJOR_VERSION}
)

target_compile_definitions(
    ${APPLICATION_NAME}
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)