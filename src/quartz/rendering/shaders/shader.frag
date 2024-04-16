#version 450

#define MAX_NUMBER_TEXTURES 100
#define MAX_NUMBER_MATERIALS 100

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

layout(binding = 3) uniform Material {
    uint baseColorTextureIndex;
    uint normalTextureIndex;
    uint emissiveTextureIndex;
    uint metallicRoughnessTextureIndex;

    vec3 baseColorFactor;
    vec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;
} materials[MAX_NUMBER_MATERIALS];

layout(binding = 4) uniform sampler textureSampler;
layout(binding = 5) uniform texture2D textures[MAX_NUMBER_TEXTURES];

layout(push_constant) uniform perObjectFragmentPushConstant {
    layout(offset = 64) uint materialIndex;
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_TBN;
layout(location = 3) in vec3 in_color;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_normalTextureCoordinate;
layout(location = 6) in vec2 in_emissiveTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Logic =====----- //

void main() {

    // ... base color ... //

    vec3 baseColor = texture(
        sampler2D(
            textures[materials[pushConstant.materialIndex].baseColorTextureIndex],
            textureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;

    // ... normal ... //

    vec3 normalDisplacement = texture(
        sampler2D(
            textures[materials[pushConstant.materialIndex].normalTextureIndex],
            textureSampler
        ),
        in_normalTextureCoordinate
    ).rgb;

    normalDisplacement = normalize((normalDisplacement * 2.0) - 1.0); // convert it to [-1, 1] from [0, 1]

    vec3 normal = normalize(in_TBN * normalDisplacement); // convert it to tangent space

    // ... ambient light ... //

    vec3 ambientLightContribution = ambientLight.color * baseColor;

    // ... directional light ... //

    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(normal, fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution =
        directionalLight.color *
        (directionalLightImpact * baseColor);

    // ... get the emissive color ... //

    vec3 emissiveColor = texture(
        sampler2D(
            textures[materials[pushConstant.materialIndex].emissiveTextureIndex],
            textureSampler
        ),
        in_emissiveTextureCoordinate
    ).rgb;

    // ... put it all together ... //

    vec3 result = ambientLightContribution + directionalLightContribution;
    result += emissiveColor;

    out_fragmentColor = vec4(result, 1.0);
}
