#version 450

layout (location = 0) in vec2 i_FragOffset;
layout (location = 0) out vec4 o_PixelColor;

struct PointLight {
    vec4 Position; // ignore w
    vec4 Color;    // w is intensity    
};

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    mat4 InverseViewMatrix;
    vec4 AmbientLightColor; // w is intensity
    PointLight PointLights[10]; // Study and use <<Specialization Constants>> instead of hardcoding.
    int ActiveLightsCount;
} ubo;

layout (push_constant) uniform Push {
    vec4 Position;
    vec4 Color;
    float Radius;
} push;

const float PI = 3.1415926538;

void main() {
    float dst = sqrt(dot(i_FragOffset, i_FragOffset)); // dot (v, v) = length(i_FragOffset^2)

    if (dst >= 1.0) {
        discard;
    }

    // https://www.desmos.com/calculator/4m470vk8ps
    o_PixelColor = vec4(push.Color.rgb, 0.5 * cos(PI * dst) + 0.5);
}

