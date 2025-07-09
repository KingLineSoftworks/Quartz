#====================================================================
# Unit tests library
#====================================================================

set(QUARTZ_UNIT_TEST_INCLUDE_DIRS ${QUARTZ_INCLUDE_DIRS} ${QUARTZ_TEST_DIR})

add_library(
    UNITTEST_Util
    Util.hpp
    Util.cpp
)
target_compile_definitions(UNITTEST_Util PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
target_compile_options(UNITTEST_Util PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
target_include_directories(UNITTEST_Util PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
target_link_libraries(
    UNITTEST_Util

    PUBLIC
    UTIL_Logger

    PUBLIC
    QUARTZ_MANAGERS_InputManager
    QUARTZ_MANAGERS_PhysicsManager
)

#====================================================================
# Math unit tests
#====================================================================

add_subdirectory("math/transform")

#====================================================================
# Util unit tests
#====================================================================

add_subdirectory("util/general")

#====================================================================
# Quartz unit tests
#====================================================================

add_subdirectory("quartz/physics/rigid_body")
add_subdirectory("quartz/scene/doodad")

