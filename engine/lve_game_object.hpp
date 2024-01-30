#pragma once

#include "lve_model.hpp"

// Libs
#include <glm/gtc/matrix_transform.hpp>

//std lib
#include <memory>

namespace lve {
    struct TransformComponent { // structs can have functions in c++
        glm::vec3 translation{}; // (position offest)
        glm::vec3 scale{1.f, 1.f, 1.f}; // (scale coefficient)
        glm::vec3 rotation{}; // (rotation angle)

        // Matrix corresponds to translate * Ry * Rx * Rz * scale
        // Rotation convention used is Y(1) X(2) Z(3)
        glm::mat4 mat4() { // 3 spacial dimensions and 1 for homogenious coordinates
            // 1 0 0 tx
            // 0 1 0 ty
            // 0 0 1 tz
            // 0 0 0 1
            auto transform = glm::translate(glm::mat4{1.f}, translation);

            transform = glm::rotate(transform, rotation.y, {0.f, 1.f, 0.f}); // transform * rotation
            transform = glm::rotate(transform, rotation.x, {1.f, 0.f, 0.f}); // transform * rotation
            transform = glm::rotate(transform, rotation.z, {0.f, 0.f, 1.f}); // transform * rotation

            transform = glm::scale(transform, scale); // transform * scale
            return transform;
        }
    };

    class LveGameObject {
        public:
        using id_t = unsigned int;

        static LveGameObject createGameObject() {
            static id_t currentId = 0;
            return LveGameObject{currentId++}; // incrementing ID every time it is created
        };

        LveGameObject(const LveGameObject &) = delete;
        LveGameObject &operator=(const LveGameObject &) = delete;
        LveGameObject(LveGameObject &&) = default; // For move to work correclty // using rvalue reference
        LveGameObject &operator=(LveGameObject &&) = default; // rvalue reference can be bound to a temporary

        const id_t getId() { return id; }

        std::shared_ptr<LveModel> model{};
        glm::vec3 color{1.f, 1.f, 1.f};
        TransformComponent transform{};

        private:
        LveGameObject(id_t objId) : id{objId} {}

        id_t id;

        
    };
} // namespace lve