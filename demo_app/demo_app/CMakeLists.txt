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
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
    "${VULKAN_ROOT_DIR}/include"
)

target_compile_options(
    ${APPLICATION_NAME}
    PUBLIC
    ${QUARTZ_CMAKE_CXX_FLAGS}
)

# Link everything to the application
target_link_libraries(
    ${APPLICATION_NAME}

    # Vendor (leaving this commented section in here for ease of placement in the future when prototyping)
    PRIVATE
    "${VULKAN_ROOT_DIR}/lib/libMoltenVK.dylib"
    "${VULKAN_ROOT_DIR}/lib/libvulkan.1.3.261.dylib"
    "${VULKAN_ROOT_DIR}/lib/libvulkan.1.dylib"
    "${VULKAN_ROOT_DIR}/lib/libvulkan.dylib"
    glfw
    glm
    # some_lib

    # Utility
    PRIVATE
    UTIL_Logger

    # Quartz
    PRIVATE
    QUARTZ_Something
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