#====================================================================
# Compile a shader to SPIR-V
#====================================================================

function(compile_shaders TARGET_NAME OUTPUT_DIR INPUT_DIR)
    set(SHADER_SOURCE_FILE_LIST ${ARGN}) # the rest of the arguments (beyond output dir)

    message(STATUS "Compiling shaders: ${SHADER_SOURCE_FILE_LIST}")
    message(STATUS "  Outputting compiled: shaders to ${OUTPUT_DIR}")

    if (NOT SHADER_SOURCE_FILE_LIST)
        message(FATAL_ERROR "Cannot compile shaders if none are given")
    endif()

    set(SHADER_COMPILATION_BYPRODUCTS)

    foreach(SHADER_SOURCE_FILE IN LISTS SHADER_SOURCE_FILE_LIST)
        message(STATUS "    Processing shader ${INPUT_DIR}/${SHADER_SOURCE_FILE}")

        cmake_path(GET SHADER_SOURCE_FILE FILENAME SHADER_SOURCE_FILENAME)
        message(STATUS "      Got filename ${SHADER_SOURCE_FILENAME}")

        set(SHADER_OUTPUT_FILENAME "${SHADER_SOURCE_FILENAME}.spv")
        set(SHADER_OUTPUT_FILE "${OUTPUT_DIR}/${SHADER_OUTPUT_FILENAME}")
        message(STATUS "      Going to create ${SHADER_OUTPUT_FILENAME} at ${SHADER_OUTPUT_FILE}")

        add_custom_command(
            OUTPUT ${SHADER_OUTPUT_FILE}
            COMMAND ${PROJECT_SOURCE_DIR}/vendor/vulkan/bin/glslc ${INPUT_DIR}/${SHADER_SOURCE_FILE} -o ${SHADER_OUTPUT_FILE}
            DEPENDS ${SHADER_SOURCE_FILE}
            COMMENT "Compiling ${INPUT_DIR}/${SHADER_SOURCE_FILE}"
            VERBATIM
        )

        list(APPEND SHADER_COMPILATION_BYPRODUCTS "${SHADER_OUTPUT_FILE}")
    endforeach()

    message(STATUS "  All compilation byproducts: ${SHADER_COMPILATION_BYPRODUCTS}")

    add_custom_target(
        ${TARGET_NAME}
        ALL
        DEPENDS ${SHADER_COMPILATION_BYPRODUCTS}
    )
endfunction()