#version 450

#define MAX_NUMBER_BASE_COLOR_TEXTURES 100

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
/** @todo 2023/12/15 Create uniform sampler for normal textures */
layout(binding = 4) uniform texture2D baseColorTextures[MAX_NUMBER_BASE_COLOR_TEXTURES];

layout(push_constant) uniform perObjectFragmentPushConstant {
    layout(offset = 64) uint baseColorTextureID;
    layout(offset = 68) uint normalTextureID;
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_fragmentTBN;
layout(location = 3) in vec3 in_fragmentColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Logic =====----- //

void main() {

    // ... base color ... //

    vec3 fragmentBaseColor = texture(
        sampler2D(
            baseColorTextures[pushConstant.baseColorTextureID],
            baseColorTextureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;

    // ... normal ... //

    // get the normal displacement from the normal map
    vec3 normalDisplacement = texture(
        sampler2D(
            baseColorTextures[pushConstant.normalTextureID],
            baseColorTextureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;

    // convert it to be [-1, 1] from [0, 1]
    normalDisplacement = normalize((normalDisplacement * 2.0) - 1.0);

    // convert it to tangent space
    vec3 normal = normalize(in_fragmentTBN * normalDisplacement);
//
//    normal = in_fragmentTBN[2];

    // ... ambient light ... //

    vec3 ambientLightContribution = ambientLight.color * fragmentBaseColor;

    // ... directional light ... //

    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(normal, fragmentToLightDirection),
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
