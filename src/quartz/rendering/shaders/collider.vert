#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ... mesh level things ... //

layout(push_constant) uniform perObjectVertexPushConstant {
    mat4 modelMatrix;
} pushConstant;

// -----==== Inputs =====----- //

/**
 * @brief This stores information about the collider. We are packing many bits into
 *   this word so we can use this as the only input.
 * 
 * @brief The information we need to transfer is:
 *    - collider type (box, sphere, capsule, custom, etc)
 *    - physics type (static, kinematic, dynamic)
 */

layout(location = 0) in vec3 in_vertexPosition;
layout(location = 1) in uint in_colliderInfoBits;

// -----==== Outputs to fragment shader =====----- //

layout(location = 0) out vec3 out_fragmentPosition;
layout(location = 1) out uint out_colliderInfoBits;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        pushConstant.modelMatrix *
        vec4(in_vertexPosition, 1.0);

    // ----- Calculate the position of the fragment ----- //

    out_fragmentPosition = vec3(pushConstant.modelMatrix * vec4(in_vertexPosition, 1.0));

    // ----- set output for fragment shader to use as input ----- //

    out_colliderInfoBits = in_colliderInfoBits;
}
