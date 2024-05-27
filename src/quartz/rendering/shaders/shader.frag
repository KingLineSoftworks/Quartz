#version 450

// ... quartz constants ... //

#define MAX_NUMBER_TEXTURES -1
#define MAX_NUMBER_MATERIALS -1

// .. math constants ... //

#define M_PI 3.1415926535897932384626433832795

// --------------------====================================== Uniforms from CPU =======================================-------------------- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

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

layout(location = 0) in vec3 in_fragmentPosition;
layout(location = 1) in mat3 in_TBN; /** @brief Tangent, Bi-Tangent, Normal vectors. All normalized */
layout(location = 4) in vec3 in_vertexColor;
layout(location = 5) in vec2 in_baseColorTextureCoordinate;
layout(location = 6) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 7) in vec2 in_normalTextureCoordinate;
layout(location = 8) in vec2 in_emissionTextureCoordinate;
layout(location = 9) in vec2 in_occlusionTextureCoordinate;

// --------------------====================================== Output =======================================-------------------- //

layout(location = 0) out vec4 out_fragmentColor;

// --------------------====================================== Helper logic declarations =======================================-------------------- //

// The only parameters these functions take in are ones that are calculated within the main function. Everything else used is a global variable

float specularBRDF(
    vec3 V, // normalized vector from fragment to camera
    vec3 L, // normalized vector from fragment to light
    vec3 N, // surface normal
    vec3 H, // half vector = normalize(L + V)
    float a // alpha = roughness
);

float diffuseBRDF();

vec3 conductorFresnel(
    vec3 V,   // normalized vector from fragment to camera
    vec3 L,   // normalized vector from fragment to light
    vec3 H,   // half vector = normalize(L + V)
    vec3 f0,  // the fragment's base color
    float bsdf // the result of the specular brdf
);

float fresnelMix(
    vec3 V,    // normalized vector from fragment to camera
    vec3 L,    // normalized vector from fragment to light
    vec3 H,    // half vector = normalize(L + V)
    float ior, // index of refraction (set to a fixed value of 1.5, a good compromise for most opaque, dielectric materials)
    vec3 base, // the result of the diffuse brdf
    float layer // the result of the specular brdf
);

vec3 getMetallicRoughnessVector();
vec3 calculateFragmentBaseColor(float roughnessValue, float metallicValue);
vec3 calculateFragmentNormal();
vec3 calculateAmbientLightContribution(vec3 fragmentBaseColor);
vec3 calculateDirectionalLightContribution(vec3 fragmentNormal, vec3 fragmentBaseColor);
vec3 calculatePointLightContribution(vec3 fragmentNormal, vec3 framentBaseColor, float roughnessValue, float metallicValue);
vec3 calculateEmissiveColorContribution();
vec4 calculateFinalColor(
    vec3 ambientLightContribution,
    vec3 directionalLightContribution,
    vec3 pointLightContribution,
    vec3 emissiveColorContribution
);

// --------------------====================================== Main logic =======================================-------------------- //

void main() {
    vec3 metallicRoughnessVector = getMetallicRoughnessVector();
    float roughnessValue = metallicRoughnessVector.g;
    float metallicValue = metallicRoughnessVector.b;
    vec3 fragmentBaseColor = calculateFragmentBaseColor(roughnessValue, metallicValue);
    vec3 fragmentNormal = calculateFragmentNormal();

    vec3 ambientLightContribution = calculateAmbientLightContribution(fragmentBaseColor);
    vec3 directionalLightContribution = calculateDirectionalLightContribution(fragmentNormal, fragmentBaseColor);
    vec3 pointLightContribution = calculatePointLightContribution(fragmentNormal, fragmentBaseColor, roughnessValue, metallicValue);
    vec3 emissiveColorContribution = calculateEmissiveColorContribution();

    out_fragmentColor = calculateFinalColor(ambientLightContribution, directionalLightContribution, pointLightContribution, emissiveColorContribution);
}

// --------------------====================================== Helper logic definitions =======================================-------------------- //

// --------------------------------------------------------------------------------
// The specular brdf function.
// This implementation was based on the explanation at https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#specular-brdf
// --------------------------------------------------------------------------------

float specularBRDF(
    vec3 V, // normalized vector from fragment to camera
    vec3 L, // normalized vector from fragment to light
    vec3 N, // surface normal
    vec3 H, // half vector = normalize(L + V)
    float a // roughness ^ 2
) {
    // @todo 2024/05/25 Should we be using a clamped dot function here instead of regular dot? //
    float a2 = a * a;
    float VdotH = dot(V, H);
    float NdotV = dot(N, V);
    float NdotL = dot(N, L);
    float NdotH = dot(N, H);
    float HdotL = dot(H, L);
    float HdotV = dot(H, V);

    if (NdotH <= 0.0 || HdotL <= 0.0 || HdotV <= 0.0) {
        return 0.0;
    }

    // ----- Calculate D ----- //

    float f = (NdotH * NdotH) * (a2 - 1.0) + 1.0;

    float D_GGX = a2 / (M_PI * f * f);

    // ----- Calculate Vis ----- //

    // @todo 2024/05/25 Quake fast square root algorithm //
    float denomL = NdotL + sqrt(a2 + (1.0 - a2) * (NdotL * NdotL));
    float denomV = NdotV + sqrt(a2 + (1.0 - a2) * (NdotV * NdotV));

    float V_GGX = (1.0 / denomL) * (1.0 / denomV);

    // ----- return ----- //

     return V_GGX * D_GGX;
}

// --------------------------------------------------------------------------------
// The lambertian diffuse brdf function.
// Taken from https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#diffuse-brdf
// --------------------------------------------------------------------------------

float diffuseBRDF() {
    return 1.0 / M_PI;
}

// --------------------------------------------------------------------------------
// The fresnel effect for metallic materials.
// Described here: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metals
// Implementation taken from: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#fresnel
// --------------------------------------------------------------------------------

vec3 conductorFresnel(
    vec3 V,   // normalized vector from fragment to camera
    vec3 L,   // normalized vector from fragment to light
    vec3 H,   // half vector = normalize(L + V)
    vec3 f0,  // the fragment's base color
    float bsdf // the result of the specular brdf
) {
    float VdotH = dot(V, H);
    float absVdotH = abs(VdotH);

    vec3 fr = f0 + (1.0 - f0) *
        (1.0 - absVdotH) * // ^1
        (1.0 - absVdotH) * // ^2
        (1.0 - absVdotH) * // ^3
        (1.0 - absVdotH) * // ^4
        (1.0 - absVdotH);  // ^5

    return bsdf * fr;
}

// --------------------------------------------------------------------------------
// The fresnel mix for dielectric materials.
// Described here: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#dielectrics
// Implementation taken from: https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#fresnel
// --------------------------------------------------------------------------------

float fresnelMix(
    vec3 V,    // normalized vector from fragment to camera
    vec3 L,    // normalized vector from fragment to light
    vec3 H,    // half vector = normalize(L + V)
    float ior, // index of refraction (set to a fixed value of 1.5 = a good compromise for most opaque, dielectric materials = gives f0 of 0.04)
    float base, // the result of the diffuse brdf
    float layer // the result of the specular brdf
) {
    float VdotH = dot(V, H);
    float absVdotH = abs(VdotH);

    float f0Base = (1 - ior) / (1 + ior);
    float f0 = f0Base * f0Base; // 0.04

    float fr = f0 + (1 - f0) *
        (1.0 - absVdotH) * // ^1
        (1.0 - absVdotH) * // ^2
        (1.0 - absVdotH) * // ^3
        (1.0 - absVdotH) * // ^4
        (1.0 - absVdotH);  // ^5

    return mix(base, layer, fr);
}


// --------------------------------------------------------------------------------
// Get the metallic and roughness values from the metallic-roughness texture.
// The roughness component is stored in the g value while the metallic value is stored in the b value.
// --------------------------------------------------------------------------------

vec3 getMetallicRoughnessVector() {
    vec3 metallicRoughnessVector = texture(
        sampler2D(textureArray[material.metallicRoughnessTextureMasterIndex], rgbaTextureSampler),
        in_metallicRoughnessTextureCoordinate
    ).rgb; // roughness in g, metallic in b

    return vec3(
        metallicRoughnessVector.r,
        metallicRoughnessVector.g * material.roughnessFactor,
        metallicRoughnessVector.b * material.metallicFactor
    );
}

// --------------------------------------------------------------------------------
// Calculate the base color of the fragment before any lighting is taken into account
// --------------------------------------------------------------------------------

vec3 calculateFragmentBaseColor(float roughnessValue, float metallicValue) {
    // @todo 2024/05/23 The base color texture MUST contain 8-bit values encoded with the sRGB opto-electronic transfer function
    //   so RGB values MUST be decoded to real linear values before they are used for any computations. To achieve correct filtering,
    //   the transfer function SHOULD be decoded before performing linear interpolation.

    vec3 fragmentBaseColor = texture(
        sampler2D(textureArray[material.baseColorTextureMasterIndex], rgbaTextureSampler),
        in_baseColorTextureCoordinate
    ).rgb;
    fragmentBaseColor *= in_vertexColor;
    fragmentBaseColor *= material.baseColorFactor.rgb;

    // @todo 2024/05/24 Actually use the metallic and roughness values in the calculation of the base color //

    if (metallicValue == 1.0) {
        // When the material is a metal, the base color is the specific measured reflectance value at normal incidence (F0).
    } else {
        // For a non-metal the base color represents the reflected diffuse color of the material.
        // In this model it is not possible to specify a F0 value for non-metals, and a linear value of 4% (0.04) is used.
    }

    return fragmentBaseColor;
}

// --------------------------------------------------------------------------------
// Calculate the normal of the fragment given the normal texture and the TBN matrix
// --------------------------------------------------------------------------------

vec3 calculateFragmentNormal() {
    vec3 normalDisplacement = texture(
        sampler2D(textureArray[material.normalTextureMasterIndex], rgbaTextureSampler),
        in_normalTextureCoordinate
    ).rgb;

    normalDisplacement = normalize((normalDisplacement * 2.0) - 1.0); // convert it to range [-1, 1] from range [0, 1]

    vec3 fragmentNormal = normalize(in_TBN * normalDisplacement); // convert the normal to tangent space and normalize it

    return fragmentNormal;
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the ambient light
// --------------------------------------------------------------------------------

vec3 calculateAmbientLightContribution(vec3 fragmentBaseColor) {
    float occlusionScale = texture(
        sampler2D(textureArray[material.occlusionTextureMasterIndex], rgbaTextureSampler),
        in_occlusionTextureCoordinate
    ).r;

    vec3 ambientLightContribution = ambientLight.color * fragmentBaseColor * occlusionScale;

    return ambientLightContribution;
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the directional light
// --------------------------------------------------------------------------------

vec3 calculateDirectionalLightContribution(vec3 fragmentNormal, vec3 fragmentBaseColor) {
    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(fragmentNormal, fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution = directionalLight.color * (directionalLightImpact * fragmentBaseColor);

    return directionalLightContribution;
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the point lights
// --------------------------------------------------------------------------------

vec3 calculatePointLightContribution(
    vec3 fragmentNormal,
    vec3 fragmentBaseColor,
    float roughnessValue,
    float metallicValue
) {
    vec3 V = normalize(camera.position - in_fragmentPosition);
    vec3 L = normalize(pointLight.position - in_fragmentPosition);
    vec3 N = fragmentNormal;
    vec3 H = normalize(L + V);

    // ... Calculate the metallic portion ... //

    float specular = specularBRDF(V, L, N, H, roughnessValue * roughnessValue);
    vec3 metallicBRDF = conductorFresnel(V, L, H, fragmentBaseColor, specular) * fragmentBaseColor;

    // ... Calculate the dielectrict portion ... //

    float ior = 1.5;
    float base = diffuseBRDF();
    float layer = specular;
    vec3 dielectricBRDF = fresnelMix(V, L, H, ior, base, layer) * fragmentBaseColor;

    // ... Mix the metallic and dielectric portions ... //

    return mix(dielectricBRDF, metallicBRDF, metallicValue);
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the emissive color
// --------------------------------------------------------------------------------

vec3 calculateEmissiveColorContribution() {
    vec3 emissiveColor = texture(
        sampler2D(textureArray[material.emissionTextureMasterIndex], rgbaTextureSampler),
        in_emissionTextureCoordinate
    ).rgb;

    return vec3(
        material.emissiveFactor.r * emissiveColor.r,
        material.emissiveFactor.g * emissiveColor.g,
        material.emissiveFactor.b * emissiveColor.b
    );
}

// --------------------------------------------------------------------------------
// Calculate the final output color
// --------------------------------------------------------------------------------

vec4 calculateFinalColor(
    vec3 ambientLightContribution,
    vec3 directionalLightContribution,
    vec3 pointLightContribution,
    vec3 emissiveColorContribution
) {
    return vec4(
        (
            ambientLightContribution +
            directionalLightContribution +
            pointLightContribution +
            emissiveColorContribution
        ),
        1.0
    );
}