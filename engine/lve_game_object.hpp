#pragma once

#include "lve_model.hpp"

//std lib
#include <memory>

namespace lve {
    struct Transform2dComponent { // structs can have functions in c++
        glm::vec2 translation{}; // (position offest)
        glm::vec2 scale{1.f, 1.f}; // (scale coefficient)
        float rotation; // 

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};
            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}}; // function to scale the matrix
            // return scaleMat * rotMatrix; // rotate, then scale
            return rotMatrix * scaleMat; // scale, then rotate (read from right to left)
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
        glm::vec3 color{};
        Transform2dComponent transform2d;

        private:
        LveGameObject(id_t objId) : id{objId} {}

        id_t id;

        
    };
} // namespace lve