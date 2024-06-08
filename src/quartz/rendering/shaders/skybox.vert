#version 450

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix; /** @todo 2024/06/08 Don't need the projection matrix in the skybox shader */
} camera;

layout(location = 0) in vec3 in_vertexPosition;

void main() {
    gl_Position = vec4(vec3(0.0), 1.0);
}
