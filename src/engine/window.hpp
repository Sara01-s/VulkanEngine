#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>

#include <string>

namespace Engine {

    class Window {

    public:
        Window(int width, int height, std::string name);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool ShouldClose() {
            return glfwWindowShouldClose(_window);
        }

        VkExtent2D GetExtent() {
            return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height) };
        }

        bool WasWindowResized() {
            return _frameBufferResized;
        }

        void ResetWindowResizeFlag() {
            _frameBufferResized = false;
        }

        void CreateWindowSurface(VkInstance vkInstance, VkSurfaceKHR* vkSurface); 

    private:
        void InitWindow();

        static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

    private:
        GLFWwindow* _window;

        int _width;
        int _height;
        bool _frameBufferResized = false;

        std::string _windowName;
    };
    
} // namespace Engine
