#version 450

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec3 a_Color;

// layout (location = 0) out vec3 o_Color;

layout (push_constant) uniform Push {
    mat2 transform;
    vec2 offset;
    vec3 color;
} push;

void main() {
//    o_Color = a_Color;
    gl_Position = vec4(push.transform * a_Position + push.offset, 0.0, 1.0);
}