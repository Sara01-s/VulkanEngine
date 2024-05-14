#pragma once

#include "device.hpp"
#include "vulkan_buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace Engine {

    class Model {

    public:
        struct Vertex {
            glm::vec3 Position;
            glm::vec3 Color;
            glm::vec3 Normal;
            glm::vec2 UV;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return Position == other.Position 
                    && Color    == other.Color 
                    && Normal   == other.Normal
                    && UV       == other.UV;
            }
        };

        struct Data {
            std::vector<Vertex> Vertices {};
            std::vector<uint32_t> Indices {};

            void LoadModel(const std::string& filepath);
        };

        Model(Device& device, const Data& data);
        ~Model();

        Model(const Model&) = delete;
        Model& operator=(const Model&) = delete;

        static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string filepath);

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);
        void CreateIndexBuffer(const std::vector<uint32_t>& indices);

    private:
        Device& _device;

        std::unique_ptr<VulkanBuffer> _vertexBuffer;
        uint32_t _vertexCount;

        bool _hasIndexBuffer { false };
        std::unique_ptr<VulkanBuffer> _indexBuffer;
        uint32_t _indexCount;
    };
    
} // namespace Engine
