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

    void App::LoadGameObjects() {
        std::shared_ptr<Model> model = Model::CreateModelFromFile(_device, "assets/models/smooth_vase.obj");

        auto gameObject = GameObject::CreateGameObject();
        gameObject.Model = model;
        gameObject.Transform.Position = { 0.0f, 0.0f, 1.0f };
        //gameObject.Transform.Scale = { 0.5f, 0.5f, 0.5f };

        _gameObjects.push_back(std::move(gameObject));
    }

} // namespace Engine
