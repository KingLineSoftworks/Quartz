#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ... mesh level things ... //

layout(push_constant) uniform perObjectVertexPushConstant {
    mat4 modelMatrix;
} pushConstant;

// -----==== Inputs =====----- //

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec3 in_vertexColor;
/** @todo 2024/05/14 All of these texture indices should be replaced with the material ID */
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 6) in vec2 in_normalTextureCoordinate;
layout(location = 7) in vec2 in_emissionTextureCoordinate;
layout(location = 8) in vec2 in_occlusionTextureCoordinate;

// -----==== Outputs to fragment shader =====----- //

layout(location = 0) out mat3 out_TBN;
layout(location = 3) out vec3 out_vertexColor;
/** @todo 2024/05/14 All of these texture indices should be replaced with the material ID */
layout(location = 4) out vec2 out_baseColorTextureCoordinate;
layout(location = 5) out vec2 out_metallicRoughnessTextureCoordinate;
layout(location = 6) out vec2 out_normalTextureCoordinate;
layout(location = 7) out vec2 out_emissionTextureCoordinate;
layout(location = 8) out vec2 out_occlusionTextureCoordinate;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        pushConstant.modelMatrix *
        vec4(in_position, 1.0);

    // ----- Calculate the TBN matrix ----- //

    /** @todo 2024/05/11 Calculate the tangent vertex attribute so we can correctly populate the TBN matrix */

    vec3 T = normalize(vec3(
        pushConstant.modelMatrix * vec4(in_tangent, 0.0)
    ));

    vec3 N = normalize(vec3(
        pushConstant.modelMatrix * vec4(in_normal, 0.0)
    ));

    vec3 B = normalize(cross(N, T));

    out_TBN = mat3(T, B, N);

    // ----- set output for fragment shader to use as input ----- //

    out_vertexColor = in_vertexColor;
    out_baseColorTextureCoordinate = in_baseColorTextureCoordinate;
    out_metallicRoughnessTextureCoordinate = in_metallicRoughnessTextureCoordinate;
    out_normalTextureCoordinate = in_normalTextureCoordinate;
    out_emissionTextureCoordinate = in_emissionTextureCoordinate;
    out_occlusionTextureCoordinate = in_occlusionTextureCoordinate;
}