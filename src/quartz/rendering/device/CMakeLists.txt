#====================================================================
# The Rendering Device library
#====================================================================
add_library(
        QUARTZ_RENDERING_Device
        SHARED
        Device.hpp
        Device.cpp
)

target_compile_options(
        QUARTZ_RENDERING_Device
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_RENDERING_Device

        PUBLIC
        vulkan

        PUBLIC
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Instance
)