#pragma once

// libs
#include "model.hpp"

// std
#include <memory>

namespace Engine {

    struct Transform2DComponent {
        glm::vec2 Translation {}; // position offset
        glm::vec2 Scale { 1.0f, 1.0f };
        float Rotation;

        glm::mat2 GetMatrix2() {
            const float s = glm::sin(Rotation);
            const float c = glm::cos(Rotation);

            glm::mat2 rotationMatrix { {c, s}, {-s, c} };
            glm::mat2 scaleMatrix { {Scale.x, 0.0f}, {0.0f, Scale.y} }; // mat constructor takes COLUMNS, not rows.

            return rotationMatrix * scaleMatrix;
        }
    };

    class GameObject {

    public:
        using ID = unsigned int;

        static GameObject CreateGameObject() {
            static ID currentId = 0;
            return GameObject { currentId++ };
        }

        GameObject(const GameObject&) = delete;
        GameObject& operator=(GameObject&) = delete;
        GameObject(GameObject&&) = default;
        GameObject& operator=(GameObject&&) = default;

        const ID GetID() const {
            return _id;
        }

    public:
        std::shared_ptr<Model> Model {};
        glm::vec3 Color {};
        Transform2DComponent Transform2D;

    private:
        GameObject(ID objId) : _id(objId) {};

    private:
        ID _id;
    };
    
} // namespace Engine
