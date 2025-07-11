#====================================================================
# Functionality for easily creating unit tests
#====================================================================

# @todo: 2025/07/09 Add flags for coverage

function(create_unit_test unit_test_name unit_test_main_file unit_test_link_libraries)
    add_executable(${unit_test_name} ${unit_test_main_file})
    target_compile_definitions(${unit_test_name} PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
    target_compile_options(${unit_test_name} PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
    target_include_directories(${unit_test_name} PRIVATE ${QUARTZ_UNIT_TEST_INCLUDE_DIRS} )
    target_link_libraries(${unit_test_name} PRIVATE ${unit_test_link_libraries} UTIL_UnitTest)
    set_target_properties(${unit_test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut)
    add_test(NAME ${unit_test_name} COMMAND ${unit_test_name})
endfunction()

