#include "point_light_system.hpp"

// std
#include <stdexcept>

namespace Engine {

    PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) 
        : _device(device)
    {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(_device.device(), _pipelineLayout, nullptr);
    }

    void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        // VkPushConstantRange pushConstantRange {};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // We want access to the pushed constant data in both vertex and fragment shader stages.
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts { globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(_device.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void PointLightSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

        PipelineConfigInfo pipelineConfig {};
        Pipeline::InitializeDefaultPipelineConfig(pipelineConfig); // it is important to use swapchains's width and height since it doesn't necessarily match the window's, on high pixel density display such as RenderSystem's retina displays, the window mesured in screen coordinates is smaller than the number of pixel, the window contains.

        pipelineConfig.RenderPass = renderPass;
        pipelineConfig.PipelineLayout = _pipelineLayout;

        _pipeline = std::make_unique<Pipeline> (
            _device,
            "assets/shaders/sh_point_light.vert.spv",
            "assets/shaders/sh_point_light.frag.spv",
            pipelineConfig
        );
    }

    void PointLightSystem::Render(FrameInfo& frameInfo) {
        _pipeline->Bind(frameInfo.CommandBuffer);

        vkCmdBindDescriptorSets (
            frameInfo.CommandBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            _pipelineLayout,
            0, 
            1,
            &frameInfo.GlobalDescriptorSet,
            0, nullptr
        );

        vkCmdDraw(frameInfo.CommandBuffer, 6, 1 ,0, 0);

    }

} // namespace Engine
