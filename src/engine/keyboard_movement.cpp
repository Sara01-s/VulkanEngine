#include "keyboard_movement.hpp"

// std
#include <limits>

namespace Engine {

    void KeyboardMovement::MoveInPlaneXZ(GLFWwindow *window, float deltaTime, GameObject &gameObject) {
        glm::vec3 rotate { 0.0f };

        if (glfwGetKey(window, Keys.LookRight) == GLFW_PRESS) {
            rotate.y += 1.0f;
        }

        if (glfwGetKey(window, Keys.LookLeft) == GLFW_PRESS) {
            rotate.y -= 1.0f;
        }

        if (glfwGetKey(window, Keys.LookUp) == GLFW_PRESS) {
            rotate.x += 1.0f;
        }

        if (glfwGetKey(window, Keys.LookDown) == GLFW_PRESS) {
            rotate.x -= 1.0f;
        }

        if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) { // rotate is not zero
            gameObject.Transform.Rotation += LookSpeed * deltaTime * glm::normalize(rotate);
        }

        gameObject.Transform.Rotation.x = glm::clamp(gameObject.Transform.Rotation.x, -1.5f, 1.5f);
        gameObject.Transform.Rotation.y = glm::mod(gameObject.Transform.Rotation.y, glm::two_pi<float>());

        float yaw = gameObject.Transform.Rotation.y;
        const glm::vec3 forwardDirection { sin(yaw), 0.0f, cos(yaw) };
        const glm::vec3 rightDirection { forwardDirection.z, 0.0f, -forwardDirection.x };
        const glm::vec3 upDirection { 0.0f, -1.0f, 0.0f };

        glm::vec3 moveDirection { 0.0f };

        if (glfwGetKey(window, Keys.MoveForward) == GLFW_PRESS) {
            moveDirection += forwardDirection;
        }

        if (glfwGetKey(window, Keys.MoveBackward) == GLFW_PRESS) {
            moveDirection += -forwardDirection;
        }
        
        if (glfwGetKey(window, Keys.MoveRight) == GLFW_PRESS) {
            moveDirection += rightDirection;
        }

        if (glfwGetKey(window, Keys.MoveLeft) == GLFW_PRESS) {
            moveDirection += -rightDirection;
        }

        if (glfwGetKey(window, Keys.MoveUp) == GLFW_PRESS) {
            moveDirection += upDirection;
        }

        if (glfwGetKey(window, Keys.MoveDown) == GLFW_PRESS) {
            moveDirection += -upDirection;
        }

        if (glm::dot(moveDirection, moveDirection) > std::numeric_limits<float>::epsilon()) { // rotate is not zero
            gameObject.Transform.Position += MoveSpeed * deltaTime * glm::normalize(moveDirection);
        }
    }
    
} // namespace Engine
