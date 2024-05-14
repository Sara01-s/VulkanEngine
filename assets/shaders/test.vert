#version 450

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec2 a_UV;

layout (location = 0) out vec3 o_Color;

layout (push_constant) uniform PushConstants { // 128 bytes, limit for supporting all GPUs
    mat4 Transform;    // projection * view * model (MVP matrix)
    mat4 NormalMatrix; // we use a mat4 for aligment rules, it will be truncated when used.
} push;

const vec3 DIRECTION_TO_LIGHT = normalize(vec3(1.0, -3.0, -1.0));
const float AMBIENT_LIGHT_INTENSITY = 0.25;

void main() {
    gl_Position = push.Transform * vec4(a_Position, 1.0);

    // This is only correct if the object is scaled uniformly.
    //vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * a_Normal); // we only need a mat3 because normals represent directions, not positions

    // calculating the inverse in a shader can be expensive and should be avoided. this will be calculated in the HOST (cpu)
    //mat3 normalMatrix = transpose(inverse(mat3(push.ModelMatrix)));
    //vec3 normalWorldSpace = normalize(push.NormalMatrix * a_Normal);

    vec3 normalWorldSpace = normalize(mat3(push.NormalMatrix) * a_Normal);

    float lightIntensity = AMBIENT_LIGHT_INTENSITY + max(dot(normalWorldSpace, DIRECTION_TO_LIGHT), 0);

    o_Color = a_Color * lightIntensity;
}