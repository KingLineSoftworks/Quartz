#version 450

// ........ math constants ........ //

#define M_PI 3.1415926535897932384626433832795

// ........ object level things ........ //

layout(push_constant) uniform perColliderFragmentPushConstant {
    layout(offset = 64) uint colliderId; // offset of 64 because vertex shader uses mat4 push constant for model matrix
} pushConstant;

// --------------------====================================== Output =======================================-------------------- //

layout(location = 0) out vec4 out_fragmentColor;

// --------------------====================================== Main logic =======================================-------------------- //

void main() {
    // @todo 2025/10/20 We should initialize an array of colors as a uniform buffer and index into it
    //    using the id modulo the array size, instead of calculating colors every frame
    float redValue = sin((pushConstant.colliderId * 1) + (M_PI / 3));
    float greenValue = sin((pushConstant.colliderId * 2) + (M_PI / 5));
    float blueValue = sin((pushConstant.colliderId * 4) + (M_PI / 7));

    out_fragmentColor = vec4(redValue, greenValue, blueValue, 0.750);
}
