#pragma once

#include "camera.hpp"
#include "game_object.hpp"

// lib
#include <vulkan/vulkan.h>

namespace Engine {

    #define MAX_LIGHTS 10

    struct PointLight {
        glm::vec4 Position {}; // ignore w
        glm::vec4 Color {}; // w is intensity
    };

    struct GlobalUBO {
        glm::mat4 ProjectionMatrix { 1.0f };
        glm::mat4 ViewMatrix { 1.0f };
        //glm::vec3 LightDirection { glm::normalize(glm::vec3 { 1.0f, -3.0f, -1.0f })};

        // using x: red, y: green, z: blue, w: intensity
        glm::vec4 AmbientColor { 1.0f, 1.0f, 1.0f, 0.25f };
        PointLight PointLights[MAX_LIGHTS];
        int ActiveLightsCount { 0 };
    };

    struct FrameInfo {
        int FrameIndex { 0 };
        float DeltaTime { 0.0f };
        VkCommandBuffer CommandBuffer;
        Camera& Camera;
        VkDescriptorSet GlobalDescriptorSet;
        GameObject::Map& GameObjectByID;
    };
    
} // namespace Engine
