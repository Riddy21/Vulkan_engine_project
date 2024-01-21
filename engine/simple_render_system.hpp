#pragma once

#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"

//std
#include <memory>
#include <vector>

namespace lve {
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            // Delete copy functions because using vulkan calls to make sure we don't have dangling pointers
            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects);

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass); // Not storing render pass, because render system lifecycle is not tied

            // Learning constructed here, that means that object will construct and deconstruct with the app
            LveDevice &lveDevice;
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
    };
}