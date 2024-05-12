#version 450

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;

layout (location = 0) out vec3 o_Color;

layout (push_constant) uniform Push {
    mat4 transform;
    vec3 color;
} push;

void main() {
    o_Color = a_Color;
    gl_Position = push.transform * vec4(a_Position, 1.0);
}