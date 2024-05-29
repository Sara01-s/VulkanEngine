#version 450

const vec2 OFFSETS[6] = vec2[] (
  vec2(-1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0, -1.0),
  vec2( 1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0,  1.0)
);

layout (location = 0) out vec2 o_FragOffset;

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

layout (push_constant) uniform Push {
    vec4 Position;
    vec4 Color;
    float Radius;
} push;


void main() {
    o_FragOffset = OFFSETS[gl_VertexIndex];

    vec3 cameraRightWorld = { ubo.ViewMatrix[0][0], ubo.ViewMatrix[1][0], ubo.ViewMatrix[2][0] };
    vec3 cameraUpWorld = { ubo.ViewMatrix[0][1], ubo.ViewMatrix[1][1], ubo.ViewMatrix[2][1] };

    vec3 vertexPositionWorld = push.Position.xyz
                                + push.Radius * o_FragOffset.x * cameraRightWorld;
                                + push.Radius * o_FragOffset.y * cameraUpWorld;

    gl_Position = ubo.ProjectionMatrix * ubo.ViewMatrix * vec4(vertexPositionWorld, 1.0);
}