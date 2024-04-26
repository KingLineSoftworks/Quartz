#====================================================================
# Compile a shader to SPIR-V
#====================================================================

function(compile_shaders TARGET_NAME OUTPUT_DIR INPUT_DIR)
    set(SHADER_SOURCE_FILE_LIST ${ARGN}) # the rest of the arguments (beyond output dir)

    message(STATUS "Compiling shaders: ${SHADER_SOURCE_FILE_LIST}")
    message(DEBUG "  Outputting compiled: shaders to ${OUTPUT_DIR}")

    if (NOT SHADER_SOURCE_FILE_LIST)
        message(FATAL_ERROR "Cannot compile shaders if none are given")
    endif()

    set(SHADER_COMPILATION_BYPRODUCTS)

    foreach(SHADER_SOURCE_FILE IN LISTS SHADER_SOURCE_FILE_LIST)
        set(SHADER_SOURCE_FULL_FILE "${INPUT_DIR}/${SHADER_SOURCE_FILE}")
        set(SHADER_SOURCE_TEMPFILE "TEMPFILE_${SHADER_SOURCE_FILE}")
        set(SHADER_SOURCE_FULL_TEMPFILE "${INPUT_DIR}/${SHADER_SOURCE_TEMPFILE}")
        message(DEBUG "    Processing shader ${SHADER_SOURCE_FULL_FILE}")
        message(DEBUG "      Using tempfile ${SHADER_SOURCE_FULL_TEMPFILE}")

        cmake_path(GET SHADER_SOURCE_FILE FILENAME SHADER_SOURCE_FILENAME)
        message(DEBUG "      Got filename ${SHADER_SOURCE_FILENAME}")

        set(SHADER_OUTPUT_FILENAME "${SHADER_SOURCE_FILENAME}.spv")
        set(SHADER_OUTPUT_FULL_FILE "${OUTPUT_DIR}/${SHADER_OUTPUT_FILENAME}")
        message(DEBUG "      Going to create ${SHADER_OUTPUT_FILENAME} at ${SHADER_OUTPUT_FILE}")

        add_custom_command(
            OUTPUT ${SHADER_OUTPUT_FULL_FILE}
            COMMAND
                cp ${SHADER_SOURCE_FULL_FILE} ${SHADER_SOURCE_FULL_TEMPFILE} &&
                sed -i.bkp "s/#define MAX_NUMBER_TEXTURES -1/#define MAX_NUMBER_TEXTURES ${MAX_NUMBER_TEXTURES}/g" ${SHADER_SOURCE_FULL_TEMPFILE} &&
                sed -i.bkp "s/#define MAX_NUMBER_MATERIALS -1/#define MAX_NUMBER_MATERIALS ${MAX_NUMBER_MATERIALS}/g" ${SHADER_SOURCE_FULL_TEMPFILE} &&
                ${PROJECT_SOURCE_DIR}/vendor/vulkan/bin/glslc ${SHADER_SOURCE_FULL_TEMPFILE} -o ${SHADER_OUTPUT_FULL_FILE} &&
                rm ${SHADER_SOURCE_FULL_TEMPFILE} &&
                rm ${SHADER_SOURCE_FULL_TEMPFILE}.bkp
            DEPENDS ${SHADER_SOURCE_FULL_FILE}
            COMMENT "Compiling ${SHADER_SOURCE_FULL_FILE}"
            VERBATIM
        )

        list(APPEND SHADER_COMPILATION_BYPRODUCTS "${SHADER_OUTPUT_FULL_FILE}")
    endforeach()

    message(DEBUG "  All compilation byproducts: ${SHADER_COMPILATION_BYPRODUCTS}")

    add_custom_target(
        ${TARGET_NAME}
        ALL
        DEPENDS ${SHADER_COMPILATION_BYPRODUCTS}
    )
endfunction()