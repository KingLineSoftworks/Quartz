#version 450

layout(binding = 2) uniform sampler2D textureSampler;

layout(location = 0) in vec3 inFragmentColor;
layout(location = 1) in vec2 inTextureCoordinate;

layout(location = 0) out vec4 outFragmentColor;

void main() {
    outFragmentColor = vec4(
        inFragmentColor * texture(textureSampler, inTextureCoordinate).rgb, 1.0
    );
}
