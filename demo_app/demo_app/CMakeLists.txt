#====================================================================
# The executable target application (the *game* oooooh)
#====================================================================
set(APPLICATION_NAME "Demo_App_The_Sequel")
set(APPLICATION_MAJOR_VERSION 0)
set(APPLICATION_MINOR_VERSION 0)
set(APPLICATION_PATCH_VERSION 0)

add_executable(
    ${APPLICATION_NAME} ${APPLICATION_SOURCE_DIR}/main.cpp
)

target_include_directories(
    ${APPLICATION_NAME}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_options(
    ${APPLICATION_NAME}
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

# Link everything to the application
target_link_libraries(
    ${APPLICATION_NAME}

    # Vendor (leaving this commented section in here for ease of placement in the future when prototyping)
    # PRIVATE
    # some_lib

    # Utility
    PRIVATE
    UTIL_Logger

    # Quartz
    # PRIVATE
    # QUARTZ_Application
)

# pre compile definitions for the target
target_compile_definitions(
    ${APPLICATION_NAME}
    PUBLIC APPLICATION_VERSION
    PUBLIC APPLICATION_MINOR_VERSION=${APPLICATION_MINOR_VERSION}
    PUBLIC APPLICATION_PATCH_VERSION=${APPLICATION_PATCH_VERSION}
    PUBLIC APPLICATION_MAJOR_VERSION=${APPLICATION_MAJOR_VERSION}
)