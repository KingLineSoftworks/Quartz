#version 450

layout(binding = 0) uniform CameraUniformBufferObject {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

layout(binding = 1) uniform ModelUniformBufferObject {
    mat4 modelMatrix;
} model;

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTextureCoordinate;

layout(location = 0) out vec3 outFragmentColor;
layout(location = 1) out vec2 outTextureCoordinate;

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        model.modelMatrix *
        vec4(inWorldPosition, 1.0)
    ;

    // ----- set output for fragment shader to use as input ----- //

    outFragmentColor = inColor;
    outTextureCoordinate = inTextureCoordinate;

}