#include "model.hpp"
#include <cassert>
#include <cstring>

namespace Engine {
    
    Model::Model(Device &device, const Data& data)
        : _device(device)
    {
        CreateVertexBuffers(data.Vertices);
        CreateIndexBuffer(data.Indices);
    }

    Model::~Model() {
        vkDestroyBuffer(_device.device(), _vertexBuffer, nullptr);
        vkFreeMemory(_device.device(), _vertexBufferMemory, nullptr);

        if (_hasIndexBuffer) {
            vkDestroyBuffer(_device.device(), _indexBuffer, nullptr);
            vkFreeMemory(_device.device(), _indexBufferMemory, nullptr);
        }
    }

    void Model::Bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { _vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (_hasIndexBuffer) {
            // uint16 = 65,535 vertices
            // uint32 = 4,294,967,295 vertices
            vkCmdBindIndexBuffer(commandBuffer, _indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::Draw(VkCommandBuffer commandBuffer) {
        if (_hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, _indexCount, 1, 0, 0, 0);
        }
        else {
            vkCmdDraw(commandBuffer, _vertexCount, 1, 0, 0);
        }
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex> &vertices) {
        _vertexCount = static_cast<uint32_t>(vertices.size());

        assert(_vertexCount >= 3 && "Vertex count must be at least 3");

        VkDeviceSize vertexBufferSize = sizeof(vertices[0]) * _vertexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        _device.createBuffer (
            vertexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // HOST = CPU, DEVICE = GPU, 1st property makes memory accesible from cpu, 2nd property keeps the cpu and gpu regions consistent (or updated) with each other, if this property is abstent, then we are required to call VkFlushMapMemoryRanges() in order to propagate changes from cpu to gpu.
            stagingBuffer,
            stagingBufferMemory
        );

        void* indexData;
        vkMapMemory(_device.device(), stagingBufferMemory, 0, vertexBufferSize, 0, &indexData);
        memcpy(indexData, vertices.data(), static_cast<size_t>(vertexBufferSize));
        vkUnmapMemory(_device.device(), stagingBufferMemory);
        
        _device.createBuffer (
            vertexBufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            _vertexBuffer,
            _vertexBufferMemory
        );

        _device.copyBuffer(stagingBuffer, _vertexBuffer, vertexBufferSize);

        vkDestroyBuffer(_device.device(), stagingBuffer, nullptr);
        vkFreeMemory(_device.device(), stagingBufferMemory, nullptr);
    }

    void Model::CreateIndexBuffer(const std::vector<uint32_t> &indices) {
        _indexCount = static_cast<uint32_t>(indices.size());
        _hasIndexBuffer = _indexCount > 0;

        if (!_hasIndexBuffer) {
            return;
        }

        VkDeviceSize indexBufferSize = sizeof(indices[0]) * _indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        _device.createBuffer (
            indexBufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // HOST = CPU, DEVICE = GPU, 1st property makes memory accesible from cpu, 2nd property keeps the cpu and gpu regions consistent (or updated) with each other, if this property is abstent, then we are required to call VkFlushMapMemoryRanges() in order to propagate changes from cpu to gpu.
            stagingBuffer,
            stagingBufferMemory
        );

        void* indexData;
        vkMapMemory(_device.device(), stagingBufferMemory, 0, indexBufferSize, 0, &indexData);
        memcpy(indexData, indices.data(), static_cast<size_t>(indexBufferSize));
        vkUnmapMemory(_device.device(), stagingBufferMemory);
        
        _device.createBuffer (
            indexBufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            _indexBuffer,
            _indexBufferMemory
        );

        _device.copyBuffer(stagingBuffer, _indexBuffer, indexBufferSize);

        vkDestroyBuffer(_device.device(), stagingBuffer, nullptr);
        vkFreeMemory(_device.device(), stagingBufferMemory, nullptr);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions { 1 };
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions { 2 }; // 2 attributes per vertex
        // position
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, Position);

        // color
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, Color);

        return attributeDescriptions;
    }

} // namespace Engine
