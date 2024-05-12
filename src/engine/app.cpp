#include "app.hpp"

#include "render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <array>
#include <iostream>

namespace Engine {

    App::App() {
        LoadGameObjects();
    }

    App::~App() 
    {
    }

    void App::Run() {
        RenderSystem renderSystem { _device, _renderer.GetSwapChainRenderPass() };

        while (!_window.ShouldClose()) {
            glfwPollEvents(); // check key strokes or window buttons (minimize, maximize, close)

            if (auto commandBuffer = _renderer.BeginFrame()) {
                _renderer.BeginSwapChainRenderPass(commandBuffer);

                renderSystem.RenderGameObjects(commandBuffer, _gameObjects);

                _renderer.EndSwapChainRenderPass(commandBuffer);
                _renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    void App::LoadGameObjects() {
        std::vector<Model::Vertex> vertices {
            { { 0.0f, -0.5f} },
            { { 0.5f,  0.5f} },
            { {-0.5f,  0.5f} }
        };

        //std::vector<Model::Vertex> vertices {};
        //Sierpinski(vertices, 3, { 0.0f, -0.5f }, { 0.5f,  0.5f }, { -0.5f,  0.5f });

        auto model = std::make_shared<Model>(_device, vertices);
        auto triangle = GameObject::CreateGameObject();
        triangle.Model = model;
        triangle.Color = { 0.1f, 0.8f, 0.1f };
        triangle.Transform2D.Translation.x = 0.2f;
        triangle.Transform2D.Scale = { 2.0f, 0.1f };
        triangle.Transform2D.Rotation = glm::two_pi<float>() * 0.25f;

        _gameObjects.push_back(std::move(triangle));
    }

} // namespace Engine
