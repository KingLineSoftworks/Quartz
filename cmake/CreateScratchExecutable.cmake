#====================================================================
# Functionality for easily creating scratch unit tests
#====================================================================

function(create_scratch_executable main_file)
    get_filename_component(executable_name ${main_file} NAME_WE)
    add_executable(${executable_name} ${main_file})

    target_compile_definitions(${executable_name} PRIVATE ${QUARTZ_COMPILE_DEFINITIONS})
    target_compile_options(${executable_name} PRIVATE ${QUARTZ_CMAKE_CXX_FLAGS})
    target_include_directories(${executable_name} PRIVATE ${QUARTZ_INCLUDE_DIRS})

    set(link_libraries ${ARGN})

    target_link_libraries(${executable_name} PRIVATE ${link_libraries} UTIL_Logger)

    set_target_properties(${executable_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/scratch)
endfunction()

