#pragma once

#include "../camera.hpp"
#include "../device.hpp"
#include "../frame_info.hpp"
#include "../game_object.hpp"
#include "../pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace Engine {

    class PointLightSystem {

    public:
        PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete;
        PointLightSystem& operator=(const PointLightSystem&) = delete;

        void Update(FrameInfo& frameInfo, GlobalUBO& ubo);
        void Render(FrameInfo& frameInfo);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);
    
    private:
        Device& _device;

        std::unique_ptr<Pipeline> _pipeline;
        VkPipelineLayout _pipelineLayout;
    };
    
} // namespace Engine
