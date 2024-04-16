#====================================================================
# The errors and exceptions utility library
#====================================================================
add_library(
        UTIL_Errors
        SHARED
        AssetErrors.cpp
        VulkanErrors.cpp
)

target_compile_options(
        UTIL_Errors
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)