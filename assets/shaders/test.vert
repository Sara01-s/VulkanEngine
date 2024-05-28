#version 450

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Color;
layout (location = 2) in vec3 a_Normal;
layout (location = 3) in vec2 a_UV;

layout (location = 0) out vec3 o_Color;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 ProjectionViewMatrix;
    vec4 AmbientLightColor; // w is intensity
    vec3 LightPosition;
    vec4 LightColor;
} ubo;

layout (push_constant) uniform PushConstants { // 128 bytes, limit for supporting all GPUs
    mat4 ModelMatrix;
    mat4 NormalMatrix; // we use a mat4 for aligment rules, it will be truncated when used.
} push;

void main() {

    // Vertex is in model space, light is in world space
    vec4 positionWorld = push.ModelMatrix * vec4(a_Position, 1.0);

    gl_Position = ubo.ProjectionViewMatrix * positionWorld;

    vec3 normalWorldSpace = normalize(mat3(push.NormalMatrix) * a_Normal);
    vec3 directionToLight = ubo.LightPosition - positionWorld.xyz;
    float lightAttenuation = 1.0 / dot(directionToLight, directionToLight); // dot(v, v) = length(v^2), do this before normalizing directionToLight

    vec3 lightColor = ubo.LightColor.rgb * ubo.LightColor.w * lightAttenuation; // scale the color by it's intensity!
    vec3 ambientLight = ubo.AmbientLightColor.rgb * ubo.AmbientLightColor.w;

    vec3 diffuseLight = lightColor * max(dot(normalWorldSpace, normalize(directionToLight)), 0);

    // This is only correct if the object is scaled uniformly.
    //vec3 normalWorldSpace = normalize(mat3(push.modelMatrix) * a_Normal); // we only need a mat3 because normals represent directions, not positions

    // calculating the inverse in a shader can be expensive and should be avoided. this will be calculated in the HOST (cpu)
    //mat3 normalMatrix = transpose(inverse(mat3(push.ModelMatrix)));
    //vec3 normalWorldSpace = normalize(push.NormalMatrix * a_Normal);

    o_Color = a_Color * (diffuseLight + ambientLight);
}