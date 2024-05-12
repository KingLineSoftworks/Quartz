#version 450

#define MAX_NUMBER_TEXTURES -1
#define MAX_NUMBER_MATERIALS -1

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
    layout(offset = 64 )uint baseColorTextureID;
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_TBN;
layout(location = 3) in vec3 in_color; /** @todo 2024/05/11 How do we use this with the base color and base color texture from the material??????? */
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 6) in vec2 in_normalTextureCoordinate;
layout(location = 7) in vec2 in_emissionTextureCoordinate;
layout(location = 8) in vec2 in_occlusionTextureCoordinate;

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

    vec3 fragmentNormal = in_TBN[2];

    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(fragmentNormal, fragmentToLightDirection),
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
