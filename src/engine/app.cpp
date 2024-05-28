#include "app.hpp"

#include "keyboard_movement.hpp"
#include "render_system.hpp"
#include "camera.hpp"
#include "vulkan_buffer.hpp"

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

    struct GlobalUBO {
        glm::mat4 ProjectionViewMatrix { 1.0f };
        //glm::vec3 LightDirection { glm::normalize(glm::vec3 { 1.0f, -3.0f, -1.0f })};

        // using x: red, y: green, z: blue, w: intensity
        glm::vec4 AmbientColor { 1.0f, 1.0f, 1.0f, 0.25f };
        glm::vec3 LightPosition { -1.0f };
        alignas(16) glm::vec4 LightColor { 1.0f };                          
    };

    constexpr float MAX_DELTA_TIME = 0.3F;

    App::App() {
        _globalPool = LveDescriptorPool::Builder(_device)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();

        LoadGameObjects();
    }

    App::~App() 
    {
    }

    void App::Run() {

        std::vector<std::unique_ptr<VulkanBuffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<VulkanBuffer> (
                _device,
                sizeof(GlobalUBO),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );

            uboBuffers[i]->map();
        }

        auto globalSetLayout = LveDescriptorSetLayout::Builder(_device)
                                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets { SwapChain::MAX_FRAMES_IN_FLIGHT };

        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();

            LveDescriptorWriter(*globalSetLayout, *_globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }
            

        RenderSystem renderSystem { _device, _renderer.GetSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
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
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.00001f, 10.0f);

            if (auto commandBuffer = _renderer.BeginFrame()) {
                int frameIndex = _renderer.GetFrameIndex();
                FrameInfo frameInfo { frameIndex, deltaTime, commandBuffer, camera, globalDescriptorSets[frameIndex] };

                // Update
                GlobalUBO ubo {};
                ubo.ProjectionViewMatrix = camera.GetProjectionMatrix() * camera.GetViewMatrix();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                // Render
                _renderer.BeginSwapChainRenderPass(commandBuffer);

                renderSystem.RenderGameObjects(frameInfo, _gameObjects);

                _renderer.EndSwapChainRenderPass(commandBuffer);
                _renderer.EndFrame();
            }
        }

        vkDeviceWaitIdle(_device.device());
    }

    void App::LoadGameObjects() {
        std::shared_ptr<Model> model = Model::CreateModelFromFile(_device, "assets/models/monkey.obj");

        auto gameObject = GameObject::CreateGameObject();
        gameObject.Model = model;
        gameObject.Transform.Position = { 0.0f, 0.0f, 1.0f };
        gameObject.Transform.Scale = { 1.0f, 1.0f, 1.0f };

        std::shared_ptr<Model> model2 = Model::CreateModelFromFile(_device, "assets/models/smooth_vase.obj");
        auto gameObject2 = GameObject::CreateGameObject();
        gameObject2.Model = model2;
        gameObject2.Transform.Position = { 1.5f, 0.0f, 1.0f };
        gameObject2.Transform.Scale = { 1.0f, 1.0f, 1.0f };

        std::shared_ptr<Model> quad = Model::CreateModelFromFile(_device, "assets/models/quad.obj");
        auto floor = GameObject::CreateGameObject();
        floor.Model = quad;
        floor.Transform.Position = { 0.0f, 0.2f, 0.0f };
        floor.Transform.Scale = { 3.0f, 1.0f, 3.0f };

        _gameObjects.push_back(std::move(gameObject));
        _gameObjects.push_back(std::move(gameObject2));
        _gameObjects.push_back(std::move(floor));
    }

} // namespace Engine
