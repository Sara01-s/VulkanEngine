#pragma once

// libs
#include "model.hpp"
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>

namespace Engine {

    struct TransformComponent {
        glm::vec3 Translation {}; // position offset
        glm::vec3 Scale { 1.0f, 1.0f, 1.0f };
        glm::vec3 Rotation;

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 GetMat4() {
            const float c3 = glm::cos(Rotation.z);
            const float s3 = glm::sin(Rotation.z);
            const float c2 = glm::cos(Rotation.x);
            const float s2 = glm::sin(Rotation.x);
            const float c1 = glm::cos(Rotation.y);
            const float s1 = glm::sin(Rotation.y);

            return glm::mat4 {
                {
                    Scale.x * (c1 * c3 + s1 * s2 * s3),
                    Scale.x * (c2 * s3),
                    Scale.x * (c1 * s2 * s3 - c3 * s1),
                    0.0f,
                },
                {
                    Scale.y * (c3 * s1 * s2 - c1 * s3),
                    Scale.y * (c2 * c3),
                    Scale.y * (c1 * c3 * s2 + s1 * s3),
                    0.0f,
                },
                {
                    Scale.z * (c2 * s1),
                    Scale.z * (-s2),
                    Scale.z * (c1 * c2),
                    0.0f,
                },
                { Translation.x, Translation.y, Translation.z, 1.0f }
                };
        }

        glm::mat4 GetMat4Slow() {
            auto transform = glm::translate(glm::mat4 { 1.0f }, Translation);

            // Tait-Bryan angles YXZ (Euler rotation)
            transform = glm::rotate(transform, Rotation.y, { 0.0f, 1.0f, 0.0f });
            transform = glm::rotate(transform, Rotation.x, { 1.0f, 0.0f, 0.0f });
            transform = glm::rotate(transform, Rotation.z, { 0.0f, 0.0f, 1.0f });

            transform = glm::scale(transform, Scale);

            return transform;
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
        TransformComponent Transform;

    private:
        GameObject(ID objId) : _id(objId) {};

    private:
        ID _id;
    };
    
} // namespace Engine
