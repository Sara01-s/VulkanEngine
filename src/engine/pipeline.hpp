#pragma once

#include "device.hpp"
#include "model.hpp"

#include <string>
#include <vector>

namespace Engine {

    struct PipelineConfigInfo {
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        std::vector<VkVertexInputBindingDescription> BindingDescriptions {};
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions {};

        VkPipelineViewportStateCreateInfo ViewportInfo;

        VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo RasterizationInfo;
        VkPipelineMultisampleStateCreateInfo MultisampleInfo;
        VkPipelineColorBlendAttachmentState ColorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;

        std::vector<VkDynamicState> DynamicStateEnables;
        VkPipelineDynamicStateCreateInfo DynamicStateInfo;

        VkPipelineLayout PipelineLayout = nullptr;
        VkRenderPass RenderPass = nullptr;
        uint32_t Subpass = 0;
    };

    class Pipeline {

    public:
        Pipeline() = default;
        Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo&);
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        Pipeline& operator=(Pipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);

        static void InitializeDefaultPipelineConfig(PipelineConfigInfo& configInfo);
    
    private:
        static std::vector<char> ReadFile(const std::string& filepath);

        void CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
        void CreateShaderModule(const std::vector<char>& sourceCode, VkShaderModule* shaderModule); 

    private:
        Device& _device;
        VkPipeline _graphicsPipeline;
        VkShaderModule _vertShaderModule;
        VkShaderModule _fragShaderModule;
    };
    
} // namespace Engine
