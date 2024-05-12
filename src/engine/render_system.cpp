#include "render_system.hpp"

// std
#include <stdexcept>

namespace Engine {

    struct SimplePushConstantData {
        glm::mat4 Transform { 1.0f };
        alignas(16) glm::vec3 Color;
    };
    
    RenderSystem::RenderSystem(Device& device, VkRenderPass renderPass) 
        : _device(device)
    {
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    RenderSystem::~RenderSystem() {
        vkDestroyPipelineLayout(_device.device(), _pipelineLayout, nullptr);
    }

    void RenderSystem::CreatePipelineLayout() {
        VkPushConstantRange pushConstantRange {};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // We want access to the pushed constant data in both vertex and fragment shader stages.
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
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

    void RenderSystem::RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects) {
        _pipeline->Bind(commandBuffer);

        for (auto& obj : gameObjects) {
            obj.Transform.Rotation.y = glm::mod(obj.Transform.Rotation.y + 0.01f, glm::two_pi<float>());
            obj.Transform.Rotation.x = glm::mod(obj.Transform.Rotation.x + 0.005f, glm::two_pi<float>());

            SimplePushConstantData push {};
            push.Color = obj.Color;
            push.Transform = obj.Transform.GetMat4();

            vkCmdPushConstants (
                commandBuffer, 
                _pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            obj.Model->Bind(commandBuffer);
            obj.Model->Draw(commandBuffer);
        }
    }

} // namespace Engine
