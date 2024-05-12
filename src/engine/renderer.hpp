#pragma once

// libs
#include "device.hpp"
#include "window.hpp"
#include "swap_chain.hpp"

// std
#include <memory>
#include <vector>
#include <cassert>

namespace Engine {
    class Renderer {

    public:
        Renderer(Window& window, Device& device);
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        VkCommandBuffer BeginFrame();
        void EndFrame();

        void BeginSwapChainRenderPass(VkCommandBuffer commandBuffer);
        void EndSwapChainRenderPass(VkCommandBuffer commandBuffer);

        VkRenderPass GetSwapChainRenderPass() const {
            return _swapChain->getRenderPass();
        }

        float GetAspectRatio() const {
            return _swapChain->extentAspectRatio();
        }

        bool IsFrameInProgress() const {
            return _isFrameStarted;
        }

        VkCommandBuffer GetCurrentCommandBuffer() const {
            assert(_isFrameStarted && "Cannot get command buffer when frame not in progress.");
            return _commandBuffers[_currentImageIndex];
        }

        int GetFrameIndex() const {
            assert(_isFrameStarted && "Cannot get frame index when frame is not in progress");
            return _currentFrameIndex;
        }

    private:
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void RecreateSwapChain();
    
    private:
        Window& _window;
        Device& _device;

        std::unique_ptr<SwapChain> _swapChain;
        std::vector<VkCommandBuffer> _commandBuffers;

        uint32_t _currentImageIndex { 0 };
        int _currentFrameIndex;
        bool _isFrameStarted { false };
    };
} // namespace Engine
