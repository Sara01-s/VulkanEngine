#pragma once

#include "camera.hpp"

// lib
#include <vulkan/vulkan.h>

namespace Engine {

    struct FrameInfo {
        int FrameIndex { 0 };
        float DeltaTime { 0.0f };
        VkCommandBuffer CommandBuffer;
        Camera& Camera;
    };
    
} // namespace Engine
