#version 450

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix; /** @todo 2024/06/08 Don't need the projection matrix in the skybox shader */
} camera;

layout(location = 0) in vec3 in_position;

layout(location = 0) out vec3 out_textureCoordinates;

void main() {
    out_textureCoordinates = in_position;
    out_textureCoordinates.xy *= -1.0; // Convert cubemap coordinates into vulkan coordinate space

    vec4 vertexPosition = camera.projectionMatrix * camera.viewMatrix * vec4(in_position, 1.0);
    gl_Position = vertexPosition.xyww;
}
