#include "render_system.hpp"

// std
#include <stdexcept>

namespace Engine {

    struct PushConstantData {
        glm::mat4 ModelMatrix { 1.0f };
        glm::mat4 NormalMatrix { 1.0f };
    };
    
    RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) 
        : _device(device)
    {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    RenderSystem::~RenderSystem() {
        vkDestroyPipelineLayout(_device.device(), _pipelineLayout, nullptr);
    }

    void RenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // We want access to the pushed constant data in both vertex and fragment shader stages.
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts { globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(_device.device(), &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void RenderSystem::CreatePipeline(VkRenderPass renderPass) {
        assert(_pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

        PipelineConfigInfo pipelineConfig {};
        Pipeline::InitializeDefaultPipelineConfig(pipelineConfig); // it is important to use swapchains's width and height since it doesn't necessarily match the window's, on high pixel density display such as RenderSystemle's retina displays, the window mesured in screen coordinates is smaller than the number of pixel, the window contains.

        pipelineConfig.RenderPass = renderPass;
        pipelineConfig.PipelineLayout = _pipelineLayout;

        _pipeline = std::make_unique<Pipeline> (
            _device,
            "assets/shaders/test.vert.spv",
            "assets/shaders/test.frag.spv",
            pipelineConfig
        );
    }

    void RenderSystem::RenderGameObjects(FrameInfo& frameInfo) {
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

        for (auto& kv : frameInfo.GameObjectByID) {
            auto& obj = kv.second; // second = value

            if (obj.Model == nullptr) {
                continue;
            }

            PushConstantData pushConstants {};
            pushConstants.ModelMatrix = obj.Transform.GetMat4();
            pushConstants.NormalMatrix = obj.Transform.GetNormalMatrix();

            vkCmdPushConstants (
                frameInfo.CommandBuffer,
                _pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PushConstantData),
                &pushConstants
            );

            obj.Model->Bind(frameInfo.CommandBuffer);
            obj.Model->Draw(frameInfo.CommandBuffer);
        }
    }

} // namespace Engine
