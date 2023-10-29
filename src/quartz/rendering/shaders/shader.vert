#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 projection;
} ubo;

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 outFragmentColor;
layout(location = 1) out vec2 outTextureCoordinate;

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        ubo.projection *
        ubo.view *
        ubo.model *
        vec4(inWorldPosition, 1.0)
    ;

    // ----- set output for fragment shader to use as input ----- //

    outFragmentColor = inColor;
    outTextureCoordinate = inTextureCoordinate;

}