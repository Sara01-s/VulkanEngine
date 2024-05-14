#pragma once

// libs
#include "device.hpp"
#include "game_object.hpp"
#include "window.hpp"
#include "renderer.hpp"

// std
#include <memory>
#include <vector>

namespace Engine {
    
    class App {

    public:
        App();
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        void Run();

    private:
        void LoadGameObjects();
    
    public:
        static constexpr int WINDOW_WIDTH = 800;
        static constexpr int WINDOW_HEIGHT = 600;

    private:
        Window _window { WINDOW_WIDTH, WINDOW_HEIGHT, "Vuwulkan Engine" };
        Device _device { _window };
        Renderer _renderer { _window, _device };

        std::vector<GameObject> _gameObjects;
    };

} // namespace Engine
