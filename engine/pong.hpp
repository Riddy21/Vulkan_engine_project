#pragma once

#include "lve_game_object.hpp"
#include "lve_model.hpp"

#include <array>

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace pong {
    class Wall {
        public:
        Wall(lve::LveDevice &device, std::array<glm::vec2, 2> location, glm::vec3 color);

        lve::LveGameObject getGameObject() { return std::move(wallGameObject); }

        private:
        std::unique_ptr<lve::LveModel> createWallModel();

        lve::LveDevice& device;
        lve::LveGameObject wallGameObject;
    };
}