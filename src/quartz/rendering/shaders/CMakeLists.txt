#====================================================================
# Compile the shaders
#====================================================================
compile_shaders(
    QUARTZ_RENDERING_Shaders
    ${PROJECT_SOURCE_DIR}       # directory to output all of the compiled shaders
    ${CMAKE_CURRENT_LIST_DIR}   # directory containing input shaders
    shader.vert
    shader.frag
)