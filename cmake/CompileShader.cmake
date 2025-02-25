#====================================================================
# Compile a shader to SPIR-V
#====================================================================

function(compile_shaders TARGET_NAME INPUT_DIR)
    set(SHADER_SOURCE_FILE_LIST ${ARGN}) # the rest of the arguments (beyond output dir)

    message(STATUS "Compiling shaders:")
    foreach(SHADER_FILE ${SHADER_SOURCE_FILE_LIST})
        message(STATUS "    ${SHADER_FILE}")
    endforeach()

    set(OUTPUT_DIR ${CMAKE_SHADER_OUTPUT_DIRECTORY})
    message(DEBUG "  Outputting compiled: shaders to ${OUTPUT_DIR}")

    if (NOT SHADER_SOURCE_FILE_LIST)
        message(FATAL_ERROR "Cannot compile shaders if none are given")
    endif()

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(GLSLC_BINARY ${PROJECT_SOURCE_DIR}/vendor/vulkan/linux/bin/glslc)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(GLSLC_BINARY ${PROJECT_SOURCE_DIR}/vendor/vulkan/mac/bin/glslc)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        message(FATAL_ERROR "Quartz is not currently supporting a build for Windows platforms")
    else()
        message(FATAL_ERROR "Quartz does not recognize the current build platform")
    endif()
    message(STATUS "Using glslc binary at ${GLSLC_BINARY}")

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
                sed -i.bkp "s/#define MAX_NUMBER_POINT_LIGHTS -1/#define MAX_NUMBER_POINT_LIGHTS ${MAX_NUMBER_POINT_LIGHTS}/g" ${SHADER_SOURCE_FULL_TEMPFILE} &&
                sed -i.bkp "s/#define MAX_NUMBER_SPOT_LIGHTS -1/#define MAX_NUMBER_SPOT_LIGHTS ${MAX_NUMBER_SPOT_LIGHTS}/g" ${SHADER_SOURCE_FULL_TEMPFILE} &&
                ${GLSLC_BINARY} ${SHADER_SOURCE_FULL_TEMPFILE} -o ${SHADER_OUTPUT_FULL_FILE} &&
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
