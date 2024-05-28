#version 450

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec2 a_UV;

layout (location = 0) out vec3 o_FragColor;
layout (location = 1) out vec3 o_FragPositionWorld;
layout (location = 2) out vec3 o_FragNormalWorld;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    vec4 AmbientLightColor; // w is intensity
    vec3 LightPosition;
    vec4 LightColor; // w is intensity
} ubo;

layout (push_constant) uniform PushConstants { // 128 bytes, limit for supporting all GPUs
    mat4 ModelMatrix;
    mat4 NormalMatrix; // we use a mat4 for aligment rules, it will be truncated when used.
} push;

void main() {

    // Vertex is in model space, light is in world space
    vec4 vertexPositionWorld = push.ModelMatrix * vec4(a_Position, 1.0);

    gl_Position = ubo.ProjectionMatrix * ubo.ViewMatrix * vertexPositionWorld;

    o_FragNormalWorld = normalize(mat3(push.NormalMatrix) * a_Normal);
    o_FragPositionWorld = vertexPositionWorld.xyz;
    o_FragColor = a_Color;
}



// Curiosities:

// This is only correct if the object is scaled uniformly.
//vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * a_Normal); // we only need a mat3 because normals represent directions, not positions

// calculating the inverse in a shader can be expensive and should be avoided. this will be calculated in the HOST (cpu)
//mat3 normalMatrix = transpose(inverse(mat3(push.ModelMatrix)));
//vec3 normalWorldSpace = normalize(push.NormalMatrix * a_Normal);