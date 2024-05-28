#include "pipeline.hpp"

#include <fstream>
#include <stdexcept>
#include <iostream>

#include <cassert>

namespace Engine {

    Pipeline::Pipeline(Device& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) 
        : _device(device)
    {
        CreateGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
    }

    Pipeline::~Pipeline() {
        vkDestroyShaderModule(_device.device(), _vertShaderModule, nullptr);
        vkDestroyShaderModule(_device.device(), _fragShaderModule, nullptr);
        vkDestroyPipeline(_device.device(), _graphicsPipeline, nullptr);
    }

    std::vector<char> Pipeline::ReadFile(const std::string &filepath) {
        std::ifstream file {filepath, std::ios::ate | std::ios::binary};

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> file_content_buffer(fileSize);

        file.seekg(0);
        file.read(file_content_buffer.data(), fileSize);
        file.close();

        return file_content_buffer;
    }

    void Pipeline::CreateGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo) {

        assert(configInfo.PipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipeline layout provided in configInfo.");
        assert(configInfo.RenderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline: no render pass provided in configInfo.");

        auto vertSourceCode = ReadFile(vertFilepath);
        auto fragSourceCode = ReadFile(fragFilepath);

        CreateShaderModule(vertSourceCode, &_vertShaderModule);
        CreateShaderModule(fragSourceCode, &_fragShaderModule);

        // Vertex shader render stage setup
        VkPipelineShaderStageCreateInfo shaderStages[2];
        shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].module = _vertShaderModule;
        shaderStages[0].pName = "main";
        shaderStages[0].flags = 0;
        shaderStages[0].pNext = nullptr;
        shaderStages[0].pSpecializationInfo = nullptr;

        // Fragment shader render stage setup
        shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].module = _fragShaderModule;
        shaderStages[1].pName = "main";
        shaderStages[1].flags = 0;
        shaderStages[1].pNext = nullptr;
        shaderStages[1].pSpecializationInfo = nullptr;

        auto bindingDescriptions = Model::Vertex::GetBindingDescriptions();
        auto attributeDescriptions = Model::Vertex::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
       
        VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2; // how many programmable stages our pipeline will use.
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &configInfo.InputAssemblyInfo;
        pipelineInfo.pViewportState = &configInfo.ViewportInfo;
        pipelineInfo.pRasterizationState = &configInfo.RasterizationInfo;
        pipelineInfo.pMultisampleState = &configInfo.MultisampleInfo;

        pipelineInfo.pColorBlendState = &configInfo.ColorBlendInfo;
        pipelineInfo.pDepthStencilState = &configInfo.DepthStencilInfo;
        pipelineInfo.pDynamicState = &configInfo.DynamicStateInfo;

        pipelineInfo.layout = configInfo.PipelineLayout;
        pipelineInfo.renderPass = configInfo.RenderPass;
        pipelineInfo.subpass = configInfo.Subpass;

        pipelineInfo.basePipelineIndex = -1;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(_device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline.");
        }

    }

    void Pipeline::CreateShaderModule(const std::vector<char>& sourceCode, VkShaderModule* shaderModule) {
        VkShaderModuleCreateInfo createInfo {};

        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = sourceCode.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data()); // an uint32_t and char are not the same size, but the vector default allocator already ensures that the data satifies the worst case aligment requirement.

        if (vkCreateShaderModule(_device.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module.");
        }
    }

    void Pipeline::Bind(VkCommandBuffer commandBuffer) {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);
    }

    void Pipeline::InitializeDefaultPipelineConfig(PipelineConfigInfo& configInfo) {

        // Input Assembly render stage setup
        configInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        configInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        configInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

        // Viewport setup
        configInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        configInfo.ViewportInfo.viewportCount = 1;
        configInfo.ViewportInfo.pViewports = nullptr;
        configInfo.ViewportInfo.scissorCount = 1;
        configInfo.ViewportInfo.pScissors = nullptr;
        
        // Rasterization render stage setup
        configInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        configInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
        configInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
        configInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        configInfo.RasterizationInfo.lineWidth = 1.0f;
        configInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        configInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
        configInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
        configInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
        configInfo.RasterizationInfo.depthBiasClamp = 0.0f;           // Optional
        configInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
        
        // MSAA Setup (ref: https://vulkan-tutorial.com/Multisampling)
        configInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        configInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
        configInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        configInfo.MultisampleInfo.minSampleShading = 1.0f;           // Optional
        configInfo.MultisampleInfo.pSampleMask = nullptr;             // Optional
        configInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
        configInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
        
        // Color blending render stage setup
        configInfo.ColorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
        configInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
        configInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        configInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        configInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
        configInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
        configInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
        configInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
        
        configInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        configInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
        configInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
        configInfo.ColorBlendInfo.attachmentCount = 1;
        configInfo.ColorBlendInfo.pAttachments = &configInfo.ColorBlendAttachment;
        configInfo.ColorBlendInfo.blendConstants[0] = 0.0f;  // Optional
        configInfo.ColorBlendInfo.blendConstants[1] = 0.0f;  // Optional
        configInfo.ColorBlendInfo.blendConstants[2] = 0.0f;  // Optional
        configInfo.ColorBlendInfo.blendConstants[3] = 0.0f;  // Optional
        
        // Stencil setup
        configInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        configInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
        configInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
        configInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        configInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
        configInfo.DepthStencilInfo.minDepthBounds = 0.0f;  // Optional
        configInfo.DepthStencilInfo.maxDepthBounds = 1.0f;  // Optional
        configInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
        configInfo.DepthStencilInfo.front = {};  // Optional
        configInfo.DepthStencilInfo.back = {};   // Optional

        // Idk setup
        configInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
        configInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        configInfo.DynamicStateInfo.pDynamicStates = configInfo.DynamicStateEnables.data();
        configInfo.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(configInfo.DynamicStateEnables.size());
        configInfo.DynamicStateInfo.flags = 0;
    }

} // namespace Engine
