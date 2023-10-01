#version 450

layout(location = 0) in vec3 inFragmentColor;

layout(location = 0) out vec4 outFragmentColor;

void main() {
    outFragmentColor = vec4(inFragmentColor, 1.0);
}
