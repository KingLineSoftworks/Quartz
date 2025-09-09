#====================================================================
# The errors and exceptions utility library
#====================================================================
add_library(
    UTIL_Errors
    SHARED
    RichException.hpp
    RichException.cpp

    AssetErrors.hpp
    AssetErrors.cpp

    VulkanErrors.hpp
    VulkanErrors.cpp
)

target_include_directories(
    UTIL_Errors
    PUBLIC
    ${QUARTZ_INCLUDE_DIRS}
)

target_compile_options(
    UTIL_Errors
    PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_compile_definitions(
    UTIL_Errors
    PUBLIC ${QUARTZ_COMPILE_DEFINITIONS}
)
