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
# Functionality for easily creating unit tests
#====================================================================

function(create_unit_test unit_test_name unit_test_main_file unit_test_link_libraries)
    add_executable(${unit_test_name} ${unit_test_main_file})
    target_compile_definitions(${unit_test_name} PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
    target_compile_options(${unit_test_name} PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
    target_include_directories(${unit_test_name} PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
    target_link_libraries(${unit_test_name} PRIVATE ${unit_test_link_libraries} UNITTEST_Util)
    add_test(NAME ${unit_test_name} COMMAND ${unit_test_name})
endfunction()

#====================================================================
# Math unit tests
#====================================================================

add_subdirectory("math/transform")

#====================================================================
# Quartz unit tests
#====================================================================

add_subdirectory("quartz/physics/rigid_body")
add_subdirectory("quartz/scene/doodad")

