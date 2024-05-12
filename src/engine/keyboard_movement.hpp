#pragma once

#include "game_object.hpp"
#include "window.hpp"

namespace Engine {

    class KeyboardMovement {

    public:
          struct KeyMappings {
            int MoveLeft     = GLFW_KEY_A;
            int MoveRight    = GLFW_KEY_D;
            int MoveForward  = GLFW_KEY_W;
            int MoveBackward = GLFW_KEY_S;
            int MoveUp       = GLFW_KEY_E;
            int MoveDown     = GLFW_KEY_Q;
            int LookLeft     = GLFW_KEY_LEFT;
            int LookRight    = GLFW_KEY_RIGHT;
            int LookUp       = GLFW_KEY_UP;
            int LookDown     = GLFW_KEY_DOWN;
        };

        void MoveInPlaneXZ(GLFWwindow* window, float deltaTime, GameObject& gameObject);

    public:
        KeyMappings Keys {};
        float MoveSpeed { 3.0f };
        float LookSpeed { 1.5f };
    };
    
} // namespace Engine
