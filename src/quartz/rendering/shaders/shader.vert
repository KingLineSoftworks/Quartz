#version 450

layout(binding = 0) uniform CameraUniformBufferObject {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

layout(binding = 1) uniform ModelUniformBufferObject {
    mat4 modelMatrix;
} model;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_nomral;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec2 in_diffuseTextureCoordinate;

layout(location = 0) out vec3 out_fragmentColor;
layout(location = 1) out vec2 out_diffuseTextureCoordinate;

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        model.modelMatrix *
        vec4(in_position, 1.0);

    // ----- set output for fragment shader to use as input ----- //

    out_fragmentColor = in_color;
    out_diffuseTextureCoordinate = in_diffuseTextureCoordinate;

}