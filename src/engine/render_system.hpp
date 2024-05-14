#pragma once

#include "camera.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace Engine {

    class RenderSystem {

    public:
        RenderSystem(Device& device, VkRenderPass renderPass);
        ~RenderSystem();

        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator=(const RenderSystem&) = delete;

        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera);

    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);
    
    private:
        Device& _device;

        std::unique_ptr<Pipeline> _pipeline;
        VkPipelineLayout _pipelineLayout;
    };
    
} // namespace Engine
