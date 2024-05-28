#version 450

const vec2 OFFSETS[6] = vec2[](
  vec2(-1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0, -1.0),
  vec2( 1.0, -1.0),
  vec2(-1.0,  1.0),
  vec2( 1.0,  1.0)
);

layout (location = 0) out vec2 o_FragOffset;

layout (set = 0, binding = 0) uniform GlobalUbo {
    mat4 ProjectionMatrix;
    mat4 ViewMatrix;
    vec4 AmbientLightColor; // w is intensity
    vec3 LightPosition;
    vec4 LightColor; // w is intensity
} ubo;

const float LIGHT_RADIUS = 0.05;

void main() {
    o_FragOffset = OFFSETS[gl_VertexIndex];

    vec3 cameraRightWorld = { ubo.ViewMatrix[0][0], ubo.ViewMatrix[1][0], ubo.ViewMatrix[2][0] };
    vec3 cameraUpWorld = { ubo.ViewMatrix[0][1], ubo.ViewMatrix[1][1], ubo.ViewMatrix[2][1] };

    vec3 positionWorld = ubo.LightPosition.xyz 
                            + LIGHT_RADIUS * o_FragOffset.x * cameraRightWorld;
                            + LIGHT_RADIUS * o_FragOffset.y * cameraUpWorld;

    gl_Position = ubo.ProjectionMatrix * (ubo.ViewMatrix * vec4(positionWorld, 1.0));
}