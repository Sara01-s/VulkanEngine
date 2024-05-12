#include "app.hpp"

#include "keyboard_movement.hpp"
#include "render_system.hpp"
#include "camera.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <stdexcept>
#include <chrono>
#include <array>
#include <iostream>

namespace Engine {

    constexpr float MAX_DELTA_TIME = 0.3F;

    App::App() {
        LoadGameObjects();
    }

    App::~App() 
    {
    }

    void App::Run() {
        RenderSystem renderSystem { _device, _renderer.GetSwapChainRenderPass() };
        Camera camera {};
        //camera.SetViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
        camera.SetViewTarget(glm::vec3 { 0.0f }, glm::vec3 { 0.0f, 0.0f, 1.0f });

        auto viewerObject = GameObject::CreateGameObject(); // camera
        KeyboardMovement cameraController {};

        auto currentTime = std::chrono::high_resolution_clock::now();

        // Game Loop
        while (!_window.ShouldClose()) {
            glfwPollEvents(); // check key strokes or window buttons (minimize, maximize, close)

            auto newTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            deltaTime = glm::min(deltaTime, MAX_DELTA_TIME);

            currentTime = newTime;

            cameraController.MoveInPlaneXZ(_window.GetWindow(), deltaTime, viewerObject);
            camera.SetViewYXZ(viewerObject.Transform.Position, viewerObject.Transform.Rotation);

            float aspectRatio = _renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 10.0f);

            if (auto commandBuffer = _renderer.BeginFrame()) {
                _renderer.BeginSwapChainRenderPass(commandBuffer);

                renderSystem.RenderGameObjects(commandBuffer, _gameObjects, camera);

                _renderer.EndSwapChainRenderPass(commandBuffer);
                _renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {

        Model::Data modelData {};

        modelData.Vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };

        for (auto& vertex : modelData.Vertices) {
            vertex.Position += offset;
        }

        modelData.Indices = { 
            0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 
        };

        return std::make_unique<Model>(device, modelData);
    }

    void App::LoadGameObjects() {
        std::shared_ptr<Model> model = createCubeModel(_device, glm::vec3 { 0.0f });

        auto cube = GameObject::CreateGameObject();
        cube.Model = model;
        cube.Transform.Position = { 0.0f, 0.0f, 1.0f };
        cube.Transform.Scale = { 0.5f, 0.5f, 0.5f };

        _gameObjects.push_back(std::move(cube));
    }

} // namespace Engine
