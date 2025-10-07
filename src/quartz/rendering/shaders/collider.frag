#version 450

// --------------------====================================== Input from vertex shader =======================================-------------------- //

layout(location = 0) in vec3 in_fragmentPosition;
layout(location = 1) flat in uint in_colliderInfoBits;

// --------------------====================================== Output =======================================-------------------- //

layout(location = 0) out vec4 out_fragmentColor;

// --------------------====================================== Main logic =======================================-------------------- //

void main() {
    out_fragmentColor = vec4(1, 1, 1, 1);
}
