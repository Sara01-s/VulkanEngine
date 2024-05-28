#version 450

layout (location = 0) in vec3 fragColor;

layout (push_constant) uniform PushConstants { // 128 bytes, limit for supporting all GPUs
    mat4 ModelMatrix;
    mat4 NormalMatrix; // we use a mat4 for aligment rules, it will be truncated when used.
} push;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}