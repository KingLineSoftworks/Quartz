#version 450

// -----==== Uniforms from the CPU =====----- //

// ... world level things ... //

layout(binding = 0) uniform CameraUniformBufferObject {
    vec3 position;
    mat4 viewMatrix;
    mat4 projectionMatrix;
} camera;

// ... mesh level things ... //

layout(push_constant) uniform perObjectVertexPushConstant {
    mat4 modelMatrix;
} pushConstant;

// -----==== Inputs =====----- //

/**
 *  @todo 2024/05/15 Can all of these texture coordinates be consolidated into one thing?
 *    We'd be sending less data to GPU but we would have to enforce all textures to be of
 *    the same dimensions, so we would have more image data, ad we wouldn't be able to
 *    combine multiple textures into one image
 */

layout(location = 0) in vec3 in_vertexPosition;
layout(location = 1) in vec3 in_vertexNormal;
layout(location = 2) in vec3 in_vertexTangent;
layout(location = 3) in vec3 in_vertexColor;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_metallicRoughnessTextureCoordinate;
layout(location = 6) in vec2 in_vertexNormalTextureCoordinate;
layout(location = 7) in vec2 in_emissionTextureCoordinate;
layout(location = 8) in vec2 in_occlusionTextureCoordinate;

// -----==== Outputs to fragment shader =====----- //

layout(location = 0) out vec3 out_fragmentPosition;
layout(location = 1) out mat3 out_TBN;
layout(location = 4) out vec3 out_vertexColor;
layout(location = 5) out vec2 out_baseColorTextureCoordinate;
layout(location = 6) out vec2 out_metallicRoughnessTextureCoordinate;
layout(location = 7) out vec2 out_normalTextureCoordinate;
layout(location = 8) out vec2 out_emissionTextureCoordinate;
layout(location = 9) out vec2 out_occlusionTextureCoordinate;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        pushConstant.modelMatrix *
        vec4(in_vertexPosition, 1.0);

    // ----- Calculate the position of the fragment ----- //

    out_fragmentPosition = vec3(pushConstant.modelMatrix * vec4(in_vertexPosition, 1.0));

    // ----- Calculate the TBN matrix ----- //

    vec3 T = normalize(vec3(
        pushConstant.modelMatrix * vec4(in_vertexTangent, 0.0)
    ));

    vec3 N = normalize(vec3(
        pushConstant.modelMatrix * vec4(in_vertexNormal, 0.0)
    ));

    T = normalize(T - dot(T, N) * N); // Re-orthogonalize T w.r.t N

    vec3 B = normalize(cross(N, T));

    out_TBN = mat3(T, B, N);

    // ----- set output for fragment shader to use as input ----- //

    out_vertexColor = in_vertexColor;
    out_baseColorTextureCoordinate = in_baseColorTextureCoordinate;
    out_metallicRoughnessTextureCoordinate = in_metallicRoughnessTextureCoordinate;
    out_normalTextureCoordinate = in_vertexNormalTextureCoordinate;
    out_emissionTextureCoordinate = in_emissionTextureCoordinate;
    out_occlusionTextureCoordinate = in_occlusionTextureCoordinate;
}