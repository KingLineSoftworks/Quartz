#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 1) uniform AmbientLight {
    vec3 color;
} ambientLight;

layout(binding = 2) uniform DirectionalLight {
    vec3 color;
    vec3 direction;
} directionalLight;

// ... object level things ... //

layout(binding = 3) uniform sampler baseColorTextureSampler;
layout(binding = 4) uniform texture2D baseColorTextures[100];

layout(push_constant) uniform perObjectFragmentPushConstant {
    layout(offset = 64)uint baseColorTextureID;
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_fragmentTBN;
layout(location = 3) in vec3 in_fragmentColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Logic =====----- //

void main() {
    vec3 fragmentBaseColor = texture(
        sampler2D(
            baseColorTextures[pushConstant.baseColorTextureID],
            baseColorTextureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;

    // ... ambient light ... //

    vec3 ambientLightContribution = ambientLight.color * fragmentBaseColor;

    // ... directional light ... //

    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(in_fragmentTBN[2], fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution =
        directionalLight.color *
        (directionalLightImpact * fragmentBaseColor);

    // ... put it all together ... //

    out_fragmentColor = vec4(
        ambientLightContribution + directionalLightContribution, 1.0
    );
}
