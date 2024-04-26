#====================================================================
# Compile the shaders
#====================================================================
compile_shaders(
    QUARTZ_RENDERING_Shaders
    ${CMAKE_CURRENT_LIST_DIR}   # directory containing input shaders
    shader.vert
    shader.frag
)