#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ... mesh level things ... //

/**
 * @brief This stores information about the collider. We are packing many bits into
 *   this word so we can use this as the only input.
 *
 * @brief The information we need to transfer is:
 *    - collider type (box, sphere, capsule, custom, etc)
 *    - physics type (static, kinematic, dynamic)
 */
layout(push_constant) uniform perObjectVertexPushConstant {
    mat4 modelMatrix;
} pushConstant;

// -----==== Inputs =====----- //

layout(location = 0) in vec3 in_vertexPosition;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position = camera.projectionMatrix * camera.viewMatrix * pushConstant.modelMatrix * vec4(in_vertexPosition, 1.0);
}
