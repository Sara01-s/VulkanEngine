#pragma once

#include "model.hpp"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace Engine {

    struct TransformComponent {
        glm::vec3 Position {}; // translation offset
        glm::vec3 Scale { 1.0f, 1.0f, 1.0f };
        glm::vec3 Rotation;

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 GetMat4();
        glm::mat4 GetMat4Slow();
        glm::mat3 GetNormalMatrix();
    };

    struct PointLightComponent {
        float LightIntensity = 1.0f;
    };

    class GameObject {

    public:
        using ID = unsigned int;
        using Map = std::unordered_map<ID, GameObject>;

        static GameObject CreateGameObject() {
            static ID currentId = 0;
            return GameObject { currentId++ };
        }

        static GameObject CreatePointLight(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.0f));

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
        TransformComponent Transform {};

        // Optional
        std::unique_ptr<PointLightComponent> PointLight = nullptr;

    private:
        GameObject(ID objId) : _id(objId) {};

    private:
        ID _id;
    };
    
} // namespace Engine
