#====================================================================
# The Application library
#====================================================================
add_library(
        QUARTZ_Application2
        SHARED
        Application2.cpp
)

target_compile_options(
        QUARTZ_Application2
        PUBLIC ${QUARTZ_CMAKE_CXX_FLAGS}
)

target_link_libraries(
        QUARTZ_Application2

        PUBLIC
#        glfw
#        glm
#        vulkan

        PUBLIC
        UTIL_FileSystem
        UTIL_Logger

        PUBLIC
        QUARTZ_RENDERING_Instance
        QUARTZ_RENDERING_Device
        QUARTZ_RENDERING_Window2
)