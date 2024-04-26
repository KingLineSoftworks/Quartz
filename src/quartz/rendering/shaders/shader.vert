#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ... mesh level things ... //

layout(push_constant) uniform perObjectVertexPushConstant {
    mat4 modelMatrix;
} pushConstant;

// -----==== Inputs =====----- //

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec2 in_baseColorTextureCoordinate;

// -----==== Outputs to fragment shader =====----- //

layout(location = 0) out vec3 out_fragmentNormal;
layout(location = 1) out vec3 out_fragmentColor;
layout(location = 2) out vec2 out_baseColorTextureCoordinate;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        pushConstant.modelMatrix *
        vec4(in_position, 1.0);

    // ----- Set the normal of the vertex ----- //

    out_fragmentNormal = normalize(
        vec3(
            pushConstant.modelMatrix * vec4(in_normal, 0.0)
        )
    );

    // ----- set output for fragment shader to use as input ----- //

    out_fragmentColor = in_color;
    out_baseColorTextureCoordinate = in_baseColorTextureCoordinate;
}