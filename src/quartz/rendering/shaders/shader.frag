#version 450

#define MAX_NUMBER_TEXTURES -1
#define MAX_NUMBER_MATERIALS -1

// --------------------====================================== Uniforms from CPU =======================================-------------------- //

// ... world level things ... //

layout(binding = 1) uniform AmbientLight {
    vec3 color;
} ambientLight;

layout(binding = 2) uniform DirectionalLight {
    vec3 color;
    vec3 direction;
} directionalLight;

layout(binding = 3) uniform PointLight {
    vec3 color;
    vec3 position;
    float attenuationConstantFactor;
    float attenuationLinearFactor;
    float attenuationQuadraticFactor;
} pointLight;

// ... object level things ... //

layout(binding = 4) uniform sampler rgbaTextureSampler;
layout(binding = 5) uniform texture2D textureArray[MAX_NUMBER_TEXTURES];

layout(binding = 6) uniform Material {
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

// --------------------====================================== Input from vertex shader =======================================-------------------- //

layout(location = 0) in mat3 in_TBN; /** @brief Tangent, Bi-Tangent, Normal vectors. All normalized */
layout(location = 3) in vec3 in_vertexColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 6) in vec2 in_normalTextureCoordinate;
layout(location = 7) in vec2 in_emissionTextureCoordinate;
layout(location = 8) in vec2 in_occlusionTextureCoordinate;

// --------------------====================================== Output =======================================-------------------- //

layout(location = 0) out vec4 out_fragmentColor;

// --------------------====================================== Helper logic declarations =======================================-------------------- //

/**
 *  @brief 2024/05/24 All of these functions use the global variables instead of taking variables in via parameters,
 *    just so we don't have to worry about copying data anywhere for the sake of speed :P might not actually make a difference
 */

vec3 getMetallicRoughnessVector();
vec3 calculateFragmentBaseColor(float roughnessValue, float metallicValue);
vec3 calculateFragmentNormal();
vec3 calculateAmbientLightContribution(vec3 fragmentBaseColor);
vec3 calculateDirectionalLightContribution(vec3 fragmentNormal, vec3 fragmentBaseColor);
vec3 calculateEmissiveColorContribution();
vec4 calculateFinalColor(
    vec3 ambientLightContribution,
    vec3 directionalLightContribution,
    vec3 emissiveColorContribution
);

// --------------------====================================== Main logic =======================================-------------------- //

void main() {
    vec3 metallicRoughnessVector = getMetallicRoughnessVector();
    vec3 fragmentBaseColor = calculateFragmentBaseColor(metallicRoughnessVector.g, metallicRoughnessVector.b);
    vec3 fragmentNormal = calculateFragmentNormal();

    vec3 ambientLightContribution = calculateAmbientLightContribution(fragmentBaseColor);
    vec3 directionalLightContribution = calculateDirectionalLightContribution(fragmentNormal, fragmentBaseColor);
    vec3 emissiveColorContribution = calculateEmissiveColorContribution();

    out_fragmentColor = calculateFinalColor(ambientLightContribution, directionalLightContribution, emissiveColorContribution);
}

// --------------------====================================== Helper logic definitions =======================================-------------------- //

/**
 *  @brief Get the metallic and roughness values from the metallic-roughness texture.
 *    The roughness component is stored in the g value while the metallic value is stored in the b value.
 */
vec3 getMetallicRoughnessVector() {
    vec3 metallicRoughnessVector = texture(
        sampler2D(textureArray[material.metallicRoughnessTextureMasterIndex], rgbaTextureSampler),
        in_metallicRoughnessTextureCoordinate
    ).rgb; // roughness in g, metallic in b

    return metallicRoughnessVector;
}

/**
 *  @brief Calculate the base color of the fragment before any lighting is taken into account
 */
vec3 calculateFragmentBaseColor(float roughnessValue, float metallicValue) {
    /**
     *  @todo 2024/05/23 The base color texture MUST contain 8-bit values encoded with the sRGB opto-electronic transfer function
     *    so RGB values MUST be decoded to real linear values before they are used for any computations. To achieve correct filtering,
     *    the transfer function SHOULD be decoded before performing linear interpolation.
     */

    vec3 fragmentBaseColor = texture(
        sampler2D(textureArray[material.baseColorTextureMasterIndex], rgbaTextureSampler),
        in_baseColorTextureCoordinate
    ).rgb;
    fragmentBaseColor *= in_vertexColor;
    fragmentBaseColor *= material.baseColorFactor.rgb;

    /** @todo 2024/05/24 Actually use the metallic and roughness values in the calculation of the base color */

    if (metallicValue == 1.0) {
        // When the material is a metal, the base color is the specific measured reflectance value at normal incidence (F0).
    } else {
        // For a non-metal the base color represents the reflected diffuse color of the material.
        // In this model it is not possible to specify a F0 value for non-metals, and a linear value of 4% (0.04) is used.
    }

    return fragmentBaseColor;
}

/**
 *  @brief Calculate the normal of the fragment given the normal texture and the TBN matrix
 */
vec3 calculateFragmentNormal() {
    vec3 normalDisplacement = texture(
        sampler2D(textureArray[material.normalTextureMasterIndex], rgbaTextureSampler),
        in_normalTextureCoordinate
    ).rgb;

    normalDisplacement = normalize((normalDisplacement * 2.0) - 1.0); // convert it to range [-1, 1] from range [0, 1]

    vec3 fragmentNormal = normalize(in_TBN * normalDisplacement); // convert the normal to tangent space and normalize it

    return fragmentNormal;
}

/**
 *  @brief Calculate the contribution to the final color from the ambient light
 */
vec3 calculateAmbientLightContribution(vec3 fragmentBaseColor) {
    float occlusionScale = texture(
        sampler2D(textureArray[material.occlusionTextureMasterIndex], rgbaTextureSampler),
        in_occlusionTextureCoordinate
    ).r;

    vec3 ambientLightContribution = ambientLight.color * fragmentBaseColor * occlusionScale;

    return ambientLightContribution;
}

/**
 *  @brief Calculate the contribution to the final color from the directional light
 */
vec3 calculateDirectionalLightContribution(vec3 fragmentNormal, vec3 fragmentBaseColor) {
    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(fragmentNormal, fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution = directionalLight.color * (directionalLightImpact * fragmentBaseColor);

    return directionalLightContribution;
}

/**
 *  @brief Calculate the contribution to the final color from the emissive color
 */
vec3 calculateEmissiveColorContribution() {
    vec3 emissiveColor = texture(
        sampler2D(textureArray[material.emissionTextureMasterIndex], rgbaTextureSampler),
        in_emissionTextureCoordinate
    ).rgb;

    return emissiveColor;
}

/**
 *  @brief Calculate the final output color
 */
vec4 calculateFinalColor(
    vec3 ambientLightContribution,
    vec3 directionalLightContribution,
    vec3 emissiveColorContribution
) {
    return vec4(
        (
            ambientLightContribution +
            directionalLightContribution +
            emissiveColorContribution
        ),
        1.0
    );
}