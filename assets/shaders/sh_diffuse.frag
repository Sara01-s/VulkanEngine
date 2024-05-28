#version 450

layout (location = 0) in vec3 i_FragColor;
layout (location = 1) in vec3 i_FragPositionWorld;
layout (location = 2) in vec3 i_FragNormalWorld;

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

layout (location = 0) out vec4 o_PixelColor;

void main() {

    vec3 directionToLight = ubo.LightPosition - i_FragPositionWorld;
    float lightAttenuation = 1.0 / dot(directionToLight, directionToLight); // dot(v, v) = length(v^2), do this before normalizing directionToLight

    vec3 lightColor = ubo.LightColor.rgb * ubo.LightColor.w * lightAttenuation; // scale the color by it's intensity!
    vec3 ambientLight = ubo.AmbientLightColor.rgb * ubo.AmbientLightColor.w;

    // The linear interpolation between two normal vectors, isn't necessarility a normal vector. So we normalize i_FragNormalWorld
    vec3 diffuseLight = lightColor * max(dot(normalize(i_FragNormalWorld), normalize(directionToLight)), 0);

    o_PixelColor = vec4((diffuseLight + ambientLight) * i_FragColor, 1.0);
}