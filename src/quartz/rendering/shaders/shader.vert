#version 450

layout(location = 0) out vec3 outFragmentColor;

vec2 positions[3] = vec2[](
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    // ----- Set the position of the vertex in clip space ----- //

    vec2 position = positions[gl_VertexIndex];
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);

    // ----- set the output fragment color for the fragment shader to use as input ----- //

    outFragmentColor = colors[gl_VertexIndex];
}