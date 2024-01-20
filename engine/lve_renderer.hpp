#pragma once

#include "lve_window.hpp"
#include "lve_device.hpp"
#include "lve_swap_chain.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>

namespace lve {
    class LveRenderer {
        public:
            LveRenderer(LveWindow &window, LveDevice &device);
            ~LveRenderer();

            // Delete copy functions because using vulkan calls to make sure we don't have dangling pointers
            LveRenderer(const LveRenderer &) = delete;
            LveRenderer &operator=(const LveRenderer &) = delete;

            // Render pass is a blueprint to tell the pipeline what frame buffer to expect
            VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass();   }
            bool isFrameInProgress() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot get frame index when frame not in progress");
                return currentFrameIndex;
            }

            // Start a frame, record whatever to the command buffer
            VkCommandBuffer beginFrame();
            // end the frame and execute
            void endFrame();

            // Need command to record swap chain's render pass
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            LveWindow& lveWindow; // Passed in from constructor
            LveDevice& lveDevice; // Passed in from constructor
            // By using unique ptr, can easily create a new swapchain and swapping it out
            std::unique_ptr<LveSwapChain> lveSwapChain;
            std::vector<VkCommandBuffer> commandBuffers; // This class manages command buffers

            // Track current state of frame in process
            uint32_t currentImageIndex = {0};
            int currentFrameIndex = {0};
            bool isFrameStarted = {false};
    };
}
