#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 2) uniform AmbientLight {
    vec3 color;
} ambientLight;

layout(binding = 3) uniform DirectionalLight {
    vec3 color;
    vec3 direction;
} directionalLight;

// ... object level things ... //

layout(binding = 4) uniform sampler2D textureSampler;

// -----==== Input from vertex shader =====----- //

layout(location = 0) in vec3 in_fragmentNormal;
layout(location = 1) in vec3 in_fragmentColor;
layout(location = 2) in vec2 in_diffuseTextureCoordinate;

// -----==== Output =====----- //

layout(location = 0) out vec4 out_fragmentColor;

// -----==== Logic =====----- //

void main() {
    vec3 fragmentDiffuseColor = vec3(
        in_fragmentColor *
        texture(textureSampler, in_diffuseTextureCoordinate).rgb
    );

    // ... ambient light ... //

    vec3 ambientLightContribution = ambientLight.color * fragmentDiffuseColor;

    // ... directional light ... //

    vec3 fragmentToLightDirection = normalize(-directionalLight.direction);

    float directionalLightImpact = max(
        dot(in_fragmentNormal, fragmentToLightDirection),
        0.0
    );

    vec3 directionalLightContribution =
        directionalLight.color *
        (directionalLightImpact * fragmentDiffuseColor);

    // ... put it all together ... //

    out_fragmentColor = vec4(
        ambientLightContribution + directionalLightContribution, 1.0
    );
}
