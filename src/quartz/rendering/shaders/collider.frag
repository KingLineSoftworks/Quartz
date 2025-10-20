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
    //
    // @brief Create a custom color based on the id of the current collider. This generates a value based on the sin function,
    //    but uses a different oscillation frequency and offset for each color channel, then it is converted from [-1, 1] to
    //    [0, 1]
    float redValue = sin((pushConstant.colliderId * 5.4321) + (M_PI / 3.0)) * 0.5 + 0.5;
    float greenValue = sin((pushConstant.colliderId * 3.4567) + (M_PI / 5.0)) * 0.5 + 0.5;
    float blueValue = sin((pushConstant.colliderId * 2.2222) + (M_PI / 7.0)) * 0.5 + 0.5;

    out_fragmentColor = vec4(redValue, greenValue, blueValue, 0.50);
}
