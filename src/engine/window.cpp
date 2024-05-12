#include "window.hpp"

#include <stdexcept>

namespace Engine {

    Window::Window(int width, int height, std::string windowName) : _width(width), _height(height), _windowName(windowName) {
        InitWindow();
    }

    Window::~Window() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void Window::CreateWindowSurface(VkInstance vkInstance, VkSurfaceKHR* vkSurface) {
        if (glfwCreateWindowSurface(vkInstance, _window, nullptr, vkSurface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface.");
        }
    }

    void Window::InitWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, FrameBufferResizeCallback);
    }

    void Window::FrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto tempWindow = reinterpret_cast<Window* >(glfwGetWindowUserPointer(window));

        tempWindow->_frameBufferResized = true;
        tempWindow->_width = width;
        tempWindow->_height = height;
    }

} // namespace Engine
