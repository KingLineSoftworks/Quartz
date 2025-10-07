#====================================================================
# Compile the shaders
#====================================================================
compile_shaders(
    QUARTZ_RENDERING_Shaders
    ${CMAKE_CURRENT_LIST_DIR}   # directory containing input shaders

    base.vert
    base.frag

    skybox.vert
    skybox.frag

    shader.vert
    shader.frag

    collider.vert
    collider.frag
)
