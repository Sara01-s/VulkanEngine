#version 450

layout (location = 0) in vec3 i_FragColor;
layout (location = 1) in vec3 i_FragPositionWorld;
layout (location = 2) in vec3 i_FragNormalWorld;

struct PointLight {
    vec4 Position; // ignore w
    vec4 Color;    // w is intensity
};

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    vec4 AmbientLightColor; // w is intensity
    PointLight PointLights[10]; // Study and use <<Specialization Constants>> instead of hardcoding.
    int ActiveLightsCount;
} ubo;

layout (push_constant) uniform PushConstants { // 128 bytes, limit for supporting all GPUs
    mat4 ModelMatrix;
    mat4 NormalMatrix; // we use a mat4 for aligment rules, it will be truncated when used.
} push;

layout (location = 0) out vec4 o_PixelColor;

void main() {
    vec3 diffuseLight = ubo.AmbientLightColor.rgb * ubo.AmbientLightColor.w;
    vec3 surfaceNormal = normalize(i_FragNormalWorld); // The linear interpolation between two normal vectors, isn't necessarility a normal vector. So we normalize i_FragNormalWorld

    for (int i = 0; i < ubo.ActiveLightsCount; i++) {
        PointLight light = ubo.PointLights[i];

        vec3 directionToLight = light.Position.xyz - i_FragPositionWorld;
        float lightAttenuation = 1.0 / dot(directionToLight, directionToLight); // dot(v, v) = length(v^2), do this before normalizing directionToLight
        float cosAngleIncidence = max(dot(surfaceNormal, normalize(directionToLight)), 0); 
        vec3 lightIntensity = light.Color.rgb * light.Color.w * lightAttenuation; // scale the color by it's intensity!

        diffuseLight += lightIntensity * cosAngleIncidence;
    }

    o_PixelColor = vec4(diffuseLight * i_FragColor, 1.0);
}