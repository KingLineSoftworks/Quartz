# ====================================================================
# Quartz project and version
# ====================================================================
set(PROJECT_NAME "Quartz")

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(QuartzVersion) # Gives us version for quartz

cmake_minimum_required(VERSION 3.14)

project(${PROJECT_NAME} VERSION ${QUARTZ_MAJOR_VERSION}.${QUARTZ_MINOR_VERSION}.${QUARTZ_PATCH_VERSION} LANGUAGES CXX C)

# Need these after `project` so we have access to CMAKE_SYSTEM_NAME
include(CompileShader) # Gives us compile_shaders function
include(CustomPedanticFlags) # Gives us the QUARTZ_CMAKE_CXX_CUSTOM_PEDANTIC_WARNING_FLAGS list

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_SHADER_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/spv)
file(MAKE_DIRECTORY ${CMAKE_SHADER_OUTPUT_DIRECTORY})

if (EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    message(STATUS "Creating compile_commands.json link in project root directory")
    file(CREATE_LINK "${CMAKE_BINARY_DIR}/compile_commands.json" "${PROJECT_SOURCE_DIR}/compile_commands.json")
else ()
    message(STATUS "CMake binary directory does not contain compile_commands.json file. This is probably the first time running cmake")
endif ()

# ====================================================================
# Build type information
# ====================================================================
# @todo: we want to have a way to specify debug for Quartz and for App separately, an example case is where we want to debug Application
# while Quartz is in release version. This will require us to have two different release mode macros (EX: QUARTZ_DEBUG , APP_DEBUG). This
# will be made easier when we move the application into it's own project which uses quartz as a submodule

message(STATUS "Generator is set to ${CMAKE_GENERATOR}")
message(STATUS "Quartz using ${CMAKE_SYSTEM_NAME} as a build platform")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} , adding DEBUG definition")
    add_compile_definitions(QUARTZ_DEBUG)

elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} , adding TEST definition")
    add_compile_definitions(QUARTZ_TEST)

elseif (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
    message(
        WARNING "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} not supported. Using CMAKE_BUILD_TYPE=Release instead , adding RELEASE definition"
    )
    set(CMAKE_BUILD_TYPE "Release")
    add_compile_definitions(QUARTZ_RELEASE)

elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} , adding RELEASE definition")
    add_compile_definitions(QUARTZ_RELEASE)

else ()
    if (CMAKE_BUILD_TYPE STREQUAL "")
        message(STATUS "CMAKE_BUILD_TYPE not provided. Using CMAKE_BUILD_TYPE=Debug instead , adding DEBUG definition")
        add_compile_definitions(QUARTZ_DEBUG)

    else ()
        message(FATAL_ERROR " - CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} is unsupported , please use Debug, RelWithDebInfo, or Release")

    endif ()

endif ()

message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")

# ====================================================================
# C++ 20 Support and compiler flags
# ====================================================================
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QUARTZ_CMAKE_CXX_DESIRED_WARNING_FLAGS -Wall -Wextra -Werror)

# Only set the c++ flags for Quartz and not for everything because we want the vendor libraries to be compiled normally
set(QUARTZ_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${QUARTZ_CMAKE_CXX_DESIRED_WARNING_FLAGS} ${QUARTZ_CMAKE_CXX_CUSTOM_PEDANTIC_WARNING_FLAGS})

# ====================================================================
# Third party vendor libraries
# ====================================================================
set(VENDOR_ROOT_DIR "${PROJECT_SOURCE_DIR}/vendor")

# glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory("${VENDOR_ROOT_DIR}/glfw")
add_compile_definitions(GLFW_INCLUDE_NONE)

# glm
add_subdirectory("${VENDOR_ROOT_DIR}/glm")
add_compile_definitions(GLM_FORCE_DEFAULT_ALIGNED_GENTYPES)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_FORCE_RADIANS)

# mikktspace
add_library(mikktspace SHARED "${VENDOR_ROOT_DIR}/MikkTSpace/mikktspace.c")
target_include_directories(mikktspace PUBLIC "${VENDOR_ROOT_DIR}/MikkTSpace")

# spdlog
set(SPDLOG_USE_STD_FORMAT ON CACHE BOOL "" FORCE)
add_subdirectory("${VENDOR_ROOT_DIR}/spdlog")

# stb
include_directories("${VENDOR_ROOT_DIR}/stb")

# tinygltf set(TINYGLTF_HEADER_ONLY ON CACHE INTERNAL "" FORCE) set(TINYGLTF_NO_INCLUDE_STB_IMAGE OFF CACHE INTERNAL "" FORCE)
# set(TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE OFF CACHE INTERNAL "" FORCE) set(TINYGLTF_USE_CPP14 ON CACHE INTERNAL "" FORCE)
set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "" FORCE)
add_subdirectory("${VENDOR_ROOT_DIR}/tinygltf")

# vulkan
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Using linux paths for vulkan includes and libraries")
    find_package(Vulkan REQUIRED)
    if (NOT Vulkan_FOUND)
        message(FATAL_ERROR "Vulkan not found")
    endif ()
    set(VULKAN_LIBRARIES "${Vulkan_LIBRARIES}")
    set(VULKAN_INCLUDE_DIRS "${Vulkan_INCLUDE_DIRS}")
    #set(VULKAN_LIBRARIES "/home/keegan/Downloads/1.2.198.1/x86_64/lib/libvulkan.so")
    #set(VULKAN_INCLUDE_DIRS "${VENDOR_ROOT_DIR}/vulkan/vulkan-headers/include")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    message(STATUS "Using mac paths for vulkan includes and libraries")
    set(VULKAN_LIBRARIES "${VENDOR_ROOT_DIR}/vulkan/mac/lib/libvulkan.dylib")
    set(VULKAN_INCLUDE_DIRS "${VENDOR_ROOT_DIR}/vulkan/mac/moltenvk-headers/include" "${VENDOR_ROOT_DIR}/vulkan/vulkan-headers/include")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message(FATAL_ERROR "Quartz is not currently supporting a build for Windows platforms")
else ()
    message(FATAL_ERROR "Quartz does not recognize ${CMAKE_SYSTEM_NAME} as a build platform")
endif ()
message(STATUS "Setting vulkan libraries: ${VULKAN_LIBRARIES}")
message(STATUS "Setting vulkan include directories: ${VULKAN_INCLUDE_DIRS}")
add_library(vulkan SHARED IMPORTED)
set_target_properties(vulkan PROPERTIES IMPORTED_LOCATION "${VULKAN_LIBRARIES}" INTERFACE_INCLUDE_DIRECTORIES "${VULKAN_INCLUDE_DIRS}")

# ====================================================================
# Define preprocessor directives for the Quartz libraries
# ====================================================================
# @todo: only add these for Quartz libraries. We don't want to be setting these for the vendor libraries. These should only be visible for
# Quartz and application

add_compile_definitions(
    PROJECT_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}" PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}"
    SHADER_BINARY_DIR="${CMAKE_SHADER_OUTPUT_DIRECTORY}"
)

set(MAX_NUMBER_TEXTURES 100)
set(MAX_NUMBER_MATERIALS 100)

set(MAX_NUMBER_POINT_LIGHTS 20)
set(MAX_NUMBER_SPOT_LIGHTS 20)

add_compile_definitions(
    QUARTZ_NAME="${PROJECT_NAME}"
    QUARTZ_VERSION
    QUARTZ_MAJOR_VERSION=${QUARTZ_MAJOR_VERSION}
    QUARTZ_MINOR_VERSION=${QUARTZ_MINOR_VERSION}
    QUARTZ_PATCH_VERSION=${QUARTZ_PATCH_VERSION}
    QUARTZ_MAX_NUMBER_TEXTURES=${MAX_NUMBER_TEXTURES}
    QUARTZ_MAX_NUMBER_MATERIALS=${MAX_NUMBER_MATERIALS}
    QUARTZ_MAX_NUMBER_POINT_LIGHTS=${MAX_NUMBER_POINT_LIGHTS}
    QUARTZ_MAX_NUMBER_SPOT_LIGHTS=${MAX_NUMBER_SPOT_LIGHTS}
)

# ====================================================================
# Utility and Quartz libraries
# ====================================================================
set(QUARTZ_ROOT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
include_directories(${QUARTZ_ROOT_SOURCE_DIR})

# Utility
set(UTIL_SOURCE_DIR "${QUARTZ_ROOT_SOURCE_DIR}/util")
add_subdirectory("${UTIL_SOURCE_DIR}/errors")
add_subdirectory("${UTIL_SOURCE_DIR}/file_system")
add_subdirectory("${UTIL_SOURCE_DIR}/logger")

# Quartz
set(QUARTZ_SOURCE_DIR "${QUARTZ_ROOT_SOURCE_DIR}/quartz")
add_subdirectory("${QUARTZ_SOURCE_DIR}/application")
add_subdirectory("${QUARTZ_SOURCE_DIR}/managers/input_manager")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/buffer")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/context")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/cube_map")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/device")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/depth_buffer")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/instance")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/material")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/model")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/pipeline")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/render_pass")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/shaders")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/swapchain")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/texture")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/vulkan_util")
add_subdirectory("${QUARTZ_SOURCE_DIR}/rendering/window")
add_subdirectory("${QUARTZ_SOURCE_DIR}/scene/camera")
add_subdirectory("${QUARTZ_SOURCE_DIR}/scene/doodad")
add_subdirectory("${QUARTZ_SOURCE_DIR}/scene/light")
add_subdirectory("${QUARTZ_SOURCE_DIR}/scene/scene")
add_subdirectory("${QUARTZ_SOURCE_DIR}/scene/sky_box")

# ====================================================================
# The demo application
# ====================================================================
set(APPLICATION_ROOT_DIR "${PROJECT_SOURCE_DIR}/demo_app")
set(APPLICATION_SOURCE_DIR "${APPLICATION_ROOT_DIR}/demo_app")
add_subdirectory("${APPLICATION_SOURCE_DIR}")
