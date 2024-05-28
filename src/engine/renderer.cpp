#include "renderer.hpp"

// std
#include <stdexcept>
#include <array>
#include <iostream>

namespace Engine {

    Renderer::Renderer(Window& window, Device& device) 
        : _window(window), _device(device)
    {
        RecreateSwapChain();
        CreateCommandBuffers();
    }

    Renderer::~Renderer() {
        FreeCommandBuffers();
    }

    void Renderer::CreateCommandBuffers() {

        _commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT + 1); // FIXME - estudiar luego, esto no debería tener + 1, pero sin él, causa un segfault

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = _device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(_commandBuffers.size());

        if (vkAllocateCommandBuffers(_device.device(), &allocInfo, _commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers.");
        }
    }

    void Renderer::FreeCommandBuffers() {
        vkFreeCommandBuffers (
            _device.device(),
            _device.getCommandPool(),
            static_cast<uint32_t>(_commandBuffers.size()),
            _commandBuffers.data()
        );

        _commandBuffers.clear();
    }

    VkCommandBuffer Renderer::BeginFrame() {
        assert(!_isFrameStarted && "Cannot call BeginFrame while already in progress");

        auto result = _swapChain->acquireNextImage(&_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) { // A surface has changed in such a way that is no longer compatible with the swapchain, Rendererlication must query the new surface properties and recreate their swapchain.
            RecreateSwapChain();
            std::cout << "Frame has not sucessfully started." << '\n';
            return nullptr; // the frame has not sucessfully started.
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            // this will hRendereren if the window gets resized.
            throw std::runtime_error("Failed to acquire swap chain image.");
        }

        _isFrameStarted = true;

        auto commandBuffer = GetCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffers.");
        }
        
        return commandBuffer;
    }

    void Renderer::EndFrame() {
        assert(_isFrameStarted && "Cannot call EndFrame while frame is not in progress");

        auto commandBuffer = GetCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer.");
        }
        
        auto result = _swapChain->submitCommandBuffers(&commandBuffer, &_currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _window.WasWindowResized()) { // A swapchain no longer matches the surface properties exactly, but CAN still be used to present.
            _window.ResetWindowResizeFlag();
            RecreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image.");
        }

        _isFrameStarted = false;
        _currentFrameIndex = (_currentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void Renderer::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(_isFrameStarted && "Cannot call BeginSwapChainRenderPass while frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _swapChain->getRenderPass();
        renderPassInfo.framebuffer = _swapChain->getFrameBuffer(_currentImageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues {};
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(_swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(_swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor { { 0, 0 }, _swapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void Renderer::EndSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(_isFrameStarted && "Cannot call EndSwapChainRenderPass while frame is not in progress");
        assert(commandBuffer == GetCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

    void Renderer::RecreateSwapChain() {
        auto extent = _window.GetExtent();

        while (extent.width == 0 || extent.height == 0) {
            extent = _window.GetExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(_device.device());

        //_swapChain = nullptr; // On some systems two swap chains can't coexist on the same window, so this ensures the old swap chain is destroyed first. This line should later be removed.
        
        if (_swapChain == nullptr) {
            _swapChain = std::make_unique<SwapChain>(_device, extent);
        }
        else {
            std::cout << "Estoy vivo" << '\n';
            std::shared_ptr<SwapChain> oldSwapChain = std::move(_swapChain);

            _swapChain = std::make_unique<SwapChain>(_device, extent, std::move(oldSwapChain));

            if (!oldSwapChain->compareSwapFormats(*_swapChain.get())) {
                // rahter than throwing an error, it will be better to setup a callback notifying that a new incompatible render pass has been created.
                throw std::runtime_error("Swap chain image (or depth) format has changed.");
            }
        }





        // we'll come back to this later.

    }

} // namespace Engine
