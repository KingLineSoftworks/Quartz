#====================================================================
# Functionality for easily creating unit tests
#====================================================================

# @todo: 2025/07/09 Add flags for coverage

function(create_unit_test main_file)
    get_filename_component(test_name ${main_file} NAME_WE)
    add_executable(${test_name} ${main_file})

    target_compile_definitions(${test_name} PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
    target_compile_options(${test_name} PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
    target_include_directories(${test_name} PRIVATE ${QUARTZ_INCLUDE_DIRS})

    set(link_libraries ${ARGN})
    target_link_libraries(${test_name} PRIVATE ${link_libraries} UTIL_UnitTest)

    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut)

    add_test(NAME ${test_name} COMMAND ${test_name} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endfunction()

