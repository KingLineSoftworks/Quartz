#version 450

// ........ quartz constants ........ //

#define MAX_NUMBER_TEXTURES -1
#define MAX_NUMBER_MATERIALS -1

#define MAX_NUMBER_POINT_LIGHTS -1
#define MAX_NUMBER_SPOT_LIGHTS -1

// ........ math constants ........ //

#define M_PI 3.1415926535897932384626433832795

// --------------------====================================== Uniforms from CPU =======================================-------------------- //

// ........ world level things ........ //

// camera //

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ambient and directional lights //

layout(binding = 1) uniform AmbientLight {
    vec3 color;
} ambientLight;

layout(binding = 2) uniform DirectionalLight {
    vec3 color;
    vec3 direction;
} directionalLight;

// point lights //

layout(binding = 3) uniform PointLightMetadata {
    uint count; // @todo 2024/05/29 We can probably put this in the PointLights uniform? //
} pointLightMetadata;

struct PointLight {
    vec3 color;
    vec3 position;
    float attenuationLinearFactor;
    float attenuationQuadraticFactor;
};

layout(binding = 4) uniform PointLights {
    PointLight array[MAX_NUMBER_POINT_LIGHTS];
} pointLights;

// spot lights //

layout(binding = 5) uniform SpotLightMetadata {
    uint count;
} spotLightMetadata;

struct SpotLight {
    vec3 color;
    vec3 position;
    vec3 direction;
    float innerRadiusDegrees;
    float outerRadiusDegrees;
    float attenuationLinearFactor;
    float attenuationQuadraticFactor;
};

layout(binding = 6) uniform SpotLights {
    SpotLight array[MAX_NUMBER_SPOT_LIGHTS];
} spotLights;

// ........ object level things ........ //

layout(binding = 7) uniform sampler rgbaTextureSampler;
layout(binding = 8) uniform texture2D textureArray[MAX_NUMBER_TEXTURES];

layout(binding = 9) uniform Material {
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

// Functions for the brdf

float calculateAttenuation(float distance, float linear, float quadratic);
vec3 calculatePointLightIntensity(PointLight pointLight);
vec3 calculateSpotLightIntensity(SpotLight spotLight);
vec3 schlickFresnel(vec3 f0, vec3 f90, vec3 l, vec3 h);
float microfacetDistributionGGX(float a, vec3 n, vec3 h);
float geometryVisibilityProbability(float ndotv, float ndotl, float roughnessValue);
vec3 diffuseBRDF(vec3 baseColor, float metallicValue);
vec3 specularBRDF(vec3 baseColor, vec3 v, vec3 l, vec3 n, vec3 h, float metallicValue, float roughnessValue);

// The only parameters these functions take in are ones that are calculated within the main function. Everything else used is a global variable

float getOcclusionScale();
vec3 getMetallicRoughnessVector();
vec3 calculateFragmentBaseColor(float roughnessValue, float metallicValue);
vec3 calculateFragmentNormal();
vec3 calculateAmbientLightContribution(vec3 fragmentBaseColor, float occlusionScale);
vec3 calculateDirectionalLightContribution(vec3 fragmentNormal, vec3 fragmentBaseColor, float occlusionScale);
vec3 calculatePointLightContribution(vec3 fragmentNormal, vec3 framentBaseColor, float occlusionScale, float roughnessValue, float metallicValue);
vec3 calculateSpotLightContribution(vec3 fragmentNormal, vec3 framentBaseColor, float occlusionScale, float roughnessValue, float metallicValue);
vec3 calculateEmissiveColorContribution();
vec4 calculateFinalColor(vec3 ambientLightContribution,  vec3 directionalLightContribution, vec3 pointLightContribution, vec3 spotLightContribution,  vec3 emissiveColorContribution);

// --------------------====================================== Main logic =======================================-------------------- //

void main() {
    float occlusionScale = getOcclusionScale();
    vec3 metallicRoughnessVector = getMetallicRoughnessVector();
    float roughnessValue = metallicRoughnessVector.g;
    float metallicValue = metallicRoughnessVector.b;
    vec3 fragmentBaseColor = calculateFragmentBaseColor(roughnessValue, metallicValue);
    vec3 fragmentNormal = calculateFragmentNormal();

    vec3 ambientLightContribution = calculateAmbientLightContribution(fragmentBaseColor, occlusionScale);
    vec3 directionalLightContribution = calculateDirectionalLightContribution(fragmentNormal, fragmentBaseColor, occlusionScale);
    vec3 pointLightContribution = calculatePointLightContribution(fragmentNormal, fragmentBaseColor, occlusionScale, roughnessValue, metallicValue);
    vec3 spotLightContribution = calculateSpotLightContribution(fragmentNormal, fragmentBaseColor, occlusionScale, roughnessValue, metallicValue);
    vec3 emissiveColorContribution = calculateEmissiveColorContribution();

    out_fragmentColor = calculateFinalColor(ambientLightContribution, directionalLightContribution, pointLightContribution, spotLightContribution, emissiveColorContribution);
}

// --------------------====================================== Helper logic definitions =======================================-------------------- //

// --------------------------------------------------------------------------------
// Calculate the intensity of the light
// @todo 2024/05/28 Determine which model to use here. Divide lightColor by distance squared? Use attenuation factors?
// --------------------------------------------------------------------------------

float calculateAttenuation(
    float d,
    float linearFactor,
    float quadraticFactor
) {
    return 1.0 / (
        1.0 + (linearFactor * d) + (quadraticFactor * d * d)
    );
}

vec3 calculatePointLightIntensity(
    PointLight pointLight
) {
    float d = length(in_fragmentPosition - pointLight.position);

    float attenuation = calculateAttenuation(d, pointLight.attenuationLinearFactor, pointLight.attenuationQuadraticFactor);

    return pointLight.color * attenuation;
}

vec3 calculateSpotLightIntensity(
    SpotLight spotLight
) {
    vec3 l = normalize(spotLight.position - in_fragmentPosition);
    float d = length(in_fragmentPosition - spotLight.position);
    float theta = dot(l, normalize(-spotLight.direction));

    float innerCutOffTheta = cos(radians(spotLight.innerRadiusDegrees));
    float outerCutOffTheta = cos(radians(spotLight.outerRadiusDegrees));
    float cutOffEpsilon = innerCutOffTheta - outerCutOffTheta;

    float radialIntensity = clamp(
        (theta - outerCutOffTheta) / cutOffEpsilon,
        0.0,
        1.0
    );

    float attenuation = calculateAttenuation(d, spotLight.attenuationLinearFactor, spotLight.attenuationQuadraticFactor);

    return spotLight.color * radialIntensity * attenuation;
}

// --------------------------------------------------------------------------------
// Schlick approximation of the Fresnel effect
// --------------------------------------------------------------------------------

vec3 schlickFresnel(
    vec3 f0,
    vec3 f90,
    vec3 l,
    vec3 h
) {
    float ldoth = dot(l, h);

    return f0 + (f90 - f0) * pow(1 - ldoth, 5);
}

// --------------------------------------------------------------------------------
// Determine the distribution of the microfacets with normals parallel to the halfway vector, h
// --------------------------------------------------------------------------------

float microfacetDistributionGGX(
    float a,
    vec3 n,
    vec3 h
) {
    float a2 = a * a;

    float ndoth = dot(n, h);
    float base = (ndoth * ndoth) * (a2 - 1.0) + 1.0;

    return a2 / (M_PI * base * base);
}

// --------------------------------------------------------------------------------
// The probability that a given microfacet is visible by both the light and the camera
// --------------------------------------------------------------------------------

float geometryVisibilityProbability(
    float ndotv,
    float ndotl,
    float roughnessValue
) {
    float k = (roughnessValue + 1.0) * (roughnessValue + 1.0) / 8;

    float GV = ndotv / (ndotv * (1.0 - k) + k);

    float GL = ndotl / (ndotl * (1.0 - k) + k);

    return GV * GL;
}

// --------------------------------------------------------------------------------
// Lambertian Diffuse
// @todo 2024/05/28 Look into using the Disney Diffuse formula
// @todo 2024/05/28 determine if we should be using pi or not
// --------------------------------------------------------------------------------

vec3 diffuseBRDF(vec3 baseColor, float metallicValue) {
    // If we have a metallic   material (1.0) then we want this to output (0.0, 0.0, 0.0)
    // If we have a dielectric material (0.0) then we want this to output (1.0, 1.0, 1.0)
    vec3 adjustedColor = mix(baseColor, vec3(0.0, 0.0, 0.0), metallicValue);

    return adjustedColor / M_PI;
}

// --------------------------------------------------------------------------------
// The specular BRDF
// --------------------------------------------------------------------------------

vec3 specularBRDF(
    vec3 baseColor,
    vec3 v,
    vec3 l,
    vec3 n,
    vec3 h,
    float metallicValue,
    float roughnessValue
) {
    float ndotv = dot(n, v);
    float ndotl = dot(n, l);

    // calculate schlick fresnel approximation
    vec3 f0 = mix(vec3(0.04, 0.04, 0.04), baseColor, metallicValue);
    vec3 f90 = vec3(1.0, 1.0, 1.0);
    vec3 F = schlickFresnel(f0, f90, l, h);

    // calculate microfacet distribution
    float D = microfacetDistributionGGX(roughnessValue * roughnessValue, n, h);

    // calculate geometric occlusion probability
    float G = geometryVisibilityProbability(ndotv, ndotl, roughnessValue);

    float denominator = 4 * ndotv * ndotl;

    return (F * D * G) / denominator;
}

// --------------------------------------------------------------------------------
// Get the diffuse occlusion scale (0.0 to 1.0)
// --------------------------------------------------------------------------------

float getOcclusionScale() {
    float occlusionScale = texture(
        sampler2D(textureArray[material.occlusionTextureMasterIndex], rgbaTextureSampler),
        in_occlusionTextureCoordinate
    ).r;

    return occlusionScale;
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

vec3 calculateAmbientLightContribution(
    vec3 fragmentBaseColor,
    float occlusionScale
) {
    vec3 ambientLightContribution = ambientLight.color * fragmentBaseColor * occlusionScale;

    return ambientLightContribution;
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the directional light
// --------------------------------------------------------------------------------

vec3 calculateDirectionalLightContribution(
    vec3 fragmentNormal,
    vec3 fragmentBaseColor,
    float occlusionScale
) {
    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(fragmentNormal, fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution = directionalLight.color * (directionalLightImpact * fragmentBaseColor) * occlusionScale;

    return directionalLightContribution;
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the point lights
// --------------------------------------------------------------------------------

vec3 calculatePointLightContribution(
    vec3 fragmentNormal,
    vec3 fragmentBaseColor,
    float occlusionScale,
    float roughnessValue,
    float metallicValue
) {
    vec3 v = normalize(camera.position - in_fragmentPosition);
    vec3 n = fragmentNormal;

    vec3 result = vec3(0.0, 0.0, 0.0);
    uint pointLightCount = min(pointLightMetadata.count, MAX_NUMBER_POINT_LIGHTS);
    for (uint i = 0; i < pointLightCount; ++i) {
        PointLight pointLight = pointLights.array[i];
        vec3 l = normalize(pointLight.position - in_fragmentPosition);
        vec3 h = normalize(l + v);

        // @todo 2024/05/29 Just give the whole point light to the function //
        vec3 intensity = calculatePointLightIntensity(pointLight);

        vec3 diffuseColor = diffuseBRDF(fragmentBaseColor, metallicValue);

        vec3 specularColor = specularBRDF(fragmentBaseColor, v, l, n, h, metallicValue, roughnessValue);

        float ndotl = clamp(dot(n, l), 0.0, 1.0);

        result += intensity * ((diffuseColor * occlusionScale) + specularColor) * ndotl;
    }

    /**
     * @todo 2024/06/08 We should figure out how to ensure that the specular brdf cannot be negative so
     *    we don't have to do any clamping here. It is likely a result of a dot product not being clamped.
     *    My main two suspects are ndotv and ndotl
     */
    return vec3(
        clamp(result.r, 0.0, 1.0),
        clamp(result.g, 0.0, 1.0),
        clamp(result.b, 0.0, 1.0)
    );
}

// --------------------------------------------------------------------------------
// Calculate the contribution to the final color from the spot lights
// --------------------------------------------------------------------------------

vec3 calculateSpotLightContribution(
    vec3 fragmentNormal,
    vec3 fragmentBaseColor,
    float occlusionScale,
    float roughnessValue,
    float metallicValue
) {
    vec3 v = normalize(camera.position - in_fragmentPosition);
    vec3 n = fragmentNormal;

    vec3 result = vec3(0.0, 0.0, 0.0);
    uint spotLightCount = min(spotLightMetadata.count, MAX_NUMBER_SPOT_LIGHTS);
    for (uint i = 0; i < spotLightCount; ++i) {
        SpotLight spotLight = spotLights.array[i];
        vec3 l = normalize(spotLight.position - in_fragmentPosition);
        vec3 h = normalize(l + v);

        vec3 intensity = calculateSpotLightIntensity(spotLight);

        vec3 diffuseColor = diffuseBRDF(fragmentBaseColor, metallicValue);

        vec3 specularColor = specularBRDF(fragmentBaseColor, v, l, n, h, metallicValue, roughnessValue);

        float ndotl = clamp(dot(n, l), 0.0, 1.0);

        result += intensity * ((diffuseColor * occlusionScale) + specularColor) * ndotl;
    }

    /**
     * @todo 2024/06/08 We should figure out how to ensure that the specular brdf cannot be negative so
     *    we don't have to do any clamping here. It is likely a result of a dot product not being clamped.
     *    My main two suspects are ndotv and ndotl
     */
    return vec3(
        clamp(result.r, 0.0, 1.0),
        clamp(result.g, 0.0, 1.0),
        clamp(result.b, 0.0, 1.0)
    );
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
    vec3 spotLightContribution,
    vec3 emissiveColorContribution
) {
    return vec4(
        (
            ambientLightContribution +
            directionalLightContribution +
            pointLightContribution +
            spotLightContribution +
            emissiveColorContribution
        ),
        1.0
    );
}