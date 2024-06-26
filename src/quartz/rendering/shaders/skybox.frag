#version 450

layout(binding = 1) uniform samplerCube cubeMap;

layout(location = 0) in vec3 in_textureCoordinates;

layout(location = 0) out vec4 out_fragmentColor;

void main() {
    out_fragmentColor = texture(cubeMap, in_textureCoordinates);
}
