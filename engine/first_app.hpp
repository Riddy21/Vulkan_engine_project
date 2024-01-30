#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class FirstApp {
        public:
            // Learning: constexpr is evaluated at compile time when possible
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FirstApp();
            ~FirstApp();

            // Delete copy functions because using vulkan calls to make sure we don't have dangling pointers
            FirstApp(const FirstApp &) = delete;
            FirstApp &operator=(const FirstApp &) = delete;
            void run();
        private:
            void loadGameObjects();
            std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset);

            // Learning constructed here, that means that object will construct and deconstruct with the app
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};
            std::vector<LveModel::Vertex> vertices;
            std::vector<LveGameObject> gameObjects;
    };
}
