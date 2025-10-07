#version 450

layout(location = 0) in vec3 in_fragmentPosition;

layout(location = 0) out vec4 out_fragmentColor;

void main() {
    out_fragmentColor = vec4(1, 0, 0, 1);
}
