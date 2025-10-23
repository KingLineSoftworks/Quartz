#version 450

layout(location = 0) in vec3 in_vertexPosition;

layout(location = 0) out vec3 out_fragmentPosition;

void main() {
    gl_Position = vec4(0, 0, 0, 0);
    out_fragmentPosition = vec3(0, 0, 0);
}
