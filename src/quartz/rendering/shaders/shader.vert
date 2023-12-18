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
layout(location = 3) in vec3 in_color;
layout(location = 4) in vec2 in_baseColorTextureCoordinate;
layout(location = 5) in vec2 in_normalTextureCoordinate;

// -----==== Outputs to fragment shader =====----- //

layout(location = 0) out mat3 out_fragmentTBN;
layout(location = 3) out vec3 out_fragmentColor;
layout(location = 4) out vec2 out_baseColorTextureCoordinate;
layout(location = 5) out vec2 out_normalTextureCoordinate;

// -----==== Logic =====----- //

void main() {

    // ----- Set the position of the vertex in clip space ----- //

    gl_Position =
        camera.projectionMatrix *
        camera.viewMatrix *
        pushConstant.modelMatrix *
        vec4(in_position, 1.0);

    // ----- Calculate the TBN matrix ----- //

    vec3 T = normalize(vec3(pushConstant.modelMatrix * vec4(in_tangent, 0.0)));
    vec3 N = normalize(vec3(pushConstant.modelMatrix * vec4(in_normal, 0.0)));

    T = normalize(T - dot(T, N) * N); // Reorthogonalize T with respect to N

    vec3 B = normalize(cross(N, T)); // Prob don't need to normalize bc T & N are normalized

    out_fragmentTBN = mat3(T, B, N);

    // ----- set output for fragment shader to use as input ----- //

    out_fragmentColor = in_color;
    out_baseColorTextureCoordinate = in_baseColorTextureCoordinate;
    out_normalTextureCoordinate = in_normalTextureCoordinate;
}