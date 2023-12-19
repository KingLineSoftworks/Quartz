#version 450

#define MAX_NUMBER_TEXTURES 100

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

layout(binding = 3) uniform sampler textureSampler;
layout(binding = 4) uniform texture2D textures[MAX_NUMBER_TEXTURES];

layout(push_constant) uniform perObjectFragmentPushConstant {
    layout(offset = 64) uint baseColorTextureID;
    layout(offset = 68) uint normalTextureID;
    layout(offset = 72) uint emissiveTextureID;
} pushConstant;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in mat3 in_fragmentTBN;
layout(location = 3) in vec3 in_fragmentColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_normalTextureCoordinate;
layout(location = 6) in vec2 in_emissiveTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Logic =====----- //

void main() {

    // ... base color ... //

    vec3 fragmentBaseColor = texture(
        sampler2D(
            textures[pushConstant.baseColorTextureID],
            textureSampler
        ),
        in_baseColorTextureCoordinate
    ).rgb;

    // ... normal ... //

    // get the normal displacement from the normal map
    vec3 normalDisplacement = texture(
        sampler2D(
            textures[pushConstant.normalTextureID],
            textureSampler
        ),
        in_normalTextureCoordinate
    ).rgb;

    // convert it to be [-1, 1] from [0, 1]
    normalDisplacement = normalize((normalDisplacement * 2.0) - 1.0);

    // convert it to tangent space
    vec3 normal = normalize(in_fragmentTBN * normalDisplacement);

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
