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

layout(binding = 3) uniform sampler rgbaTextureSampler;
layout(binding = 4) uniform texture2D textureArray[MAX_NUMBER_TEXTURES];

layout(binding = 5) uniform Material {
    uint baseColorTextureMasterIndex;
    uint metallicRoughnessTextureMasterIndex;
    uint normalTextureMasterIndex;
    uint emissionTextureMasterIndex;
    uint occlusionTextureMasterIndex;

    vec4 baseColorFactor;
    vec3 emissiveFactor;
    float metallicFactor;
    float roughnessFactor;

    uint alphaMode;     /** 0 = Opaque , 1 = Mask , 2 = Blend | https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#alpha-coverage */
    float alphaCutoff;   /** Only used when alpha mode is Mask */
    uint doubleSided;
} material;

/** @brief 2024/05/16 This isn't actually used for anything and is just here as an example of using a push constant in the fragment shader */
layout(push_constant) uniform perObjectFragmentPushConstant {
    layout(offset = 64) uint materialMasterIndex; // offset of 64 because vertex shader uses mat4 push constant for model matrix
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_TBN; /** @brief Tangent, Bi-Tangent, Normal vectors. All normalized */
layout(location = 3) in vec3 in_vertexColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 6) in vec2 in_normalTextureCoordinate;
layout(location = 7) in vec2 in_emissionTextureCoordinate;
layout(location = 8) in vec2 in_occlusionTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Helper Logic =====----- //

// -----==== Main Logic =====----- //

void main() {

    // ... base color ... //

    vec3 fragmentBaseColor = texture(
        sampler2D(
            textureArray[material.baseColorTextureMasterIndex],
            rgbaTextureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;
    fragmentBaseColor *= in_vertexColor;
    fragmentBaseColor *= material.baseColorFactor.rgb;

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

    // ... emissive contribution ... //

    vec3 emissiveColor = texture(
        sampler2D(
            textureArray[material.emissionTextureMasterIndex],
            rgbaTextureSampler
        ),
        in_emissionTextureCoordinate
    ).rgb;

    // ... put it all together ... //

    out_fragmentColor = vec4(
        ambientLightContribution +
        directionalLightContribution +
        emissiveColor,

        1.0
    );
}
