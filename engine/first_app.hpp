#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"
#include "lve_model.hpp"

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
            void loadModels();
            std::vector<LveModel::Vertex> draw_triangles(std::vector<LveModel::Vertex> input, unsigned int depth);
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);

            // Learning constructed here, that means that object will construct and deconstruct with the app
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            // By using unique ptr, can easily create a new swapchain and swapping it out
            std::unique_ptr<LveSwapChain> lveSwapChain;
            // Smart pointer:
            //  A pointer with the additional functionality of automatic memory management
            //  No longer responsible for new and delete
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LveModel> lveModel;

            std::vector<LveModel::Vertex> vertices;
    };
}
