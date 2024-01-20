#include "lve_renderer.hpp"

#include <stdexcept>

namespace lve {

    LveRenderer::LveRenderer(LveWindow &window, LveDevice &device) : lveWindow{window}, lveDevice{device} {
        recreateSwapChain();
        createCommandBuffers();
    }

    LveRenderer::~LveRenderer() {
        freeCommandBuffers(); // Is possible that the application will continue when the renderer is destroyed
    }

    void LveRenderer::recreateSwapChain() {
        auto extent = lveWindow.getExtent();
        // Causes program to freeze when minimized
        // This is when one dimension is 0
        while (extent.width == 0 || extent.height == 0) {
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lveDevice.device());

        if (lveSwapChain == nullptr)
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        else
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move(lveSwapChain)); // Allow us to create a new copy of lveSwapChain, but set LveSwapChain as null pointer
            if (lveSwapChain->imageCount() != commandBuffers.size() && commandBuffers.size() > 0) {
                freeCommandBuffers();
                createCommandBuffers();
            }

        // TODO
    }

    /*
    More about command buffers
    Not able to execute commands directly on the GPU
    Submit buffer to device queue to be submitted to a framebuffer
    Command buffers allow a sequence of commands to be recorded once
        Record once at initiiation, then execute multiple times


    Lifecycle
        Executes, and the in a pending state
        CPU will wait for frame buffer to stop executing before reusing
    */

    void LveRenderer::createCommandBuffers() {
        // Depends on whether computer supports double or triple buffering
        // Will match the number of framebuffer
        commandBuffers.resize(lveSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // Types of command buffers
        // Primary:
        //      - allows you to submit to the graphics queue
        //      - cannot be called by other buffers
        // Secondary: 
        //      - Cannot be submitted
        //      - can be called by other buffers
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // Command pool is a opaque objects command buffers memory is allocated from
        //      Command buffers may need to be destroyed frequently
        //      Memory reallocation only needed to be done once and redistributed by device pool
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        // Allocates space for command buffers
        if (vkAllocateCommandBuffers(lveDevice.device(),
                                     &allocInfo,
                                     commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void LveRenderer::freeCommandBuffers() {
        vkFreeCommandBuffers(
            lveDevice.device(),
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    // Begin frame gets the right image from the swap chain render pass
    VkCommandBuffer LveRenderer::beginFrame() {
        // Frame can't have started
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        // fetches index of frame rendered next
        // Handles CPU GPU sync for v-sync
        auto result = lveSwapChain->acquireNextImage(&currentImageIndex);

        // Detect if the window has been resized
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {// Detect if surface changed in such way that it is incompatible with swap chain
            recreateSwapChain();
            return nullptr; // Indicate has not successfully started
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            // Need to handle suboptimal case in the future due to window resizing
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Begin recording a command buffer
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        return commandBuffer;

    }
    void LveRenderer::endFrame() {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }

        // submits the command buffer to the device graphics queue, handle CPU-GPU sync
        // Command buffer will be executed
        // Submit to the display at the appropriate time
        auto result = lveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        // Detect after command buffer if it has been resized
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        isFrameStarted = false;
    }

    void LveRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
               "Can't begin render pass on command buffer from different frame");
            
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // First command to begin a render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(currentImageIndex); // writing to a specific frame buffer

        // Setup the render area, area where shaders loads and stores take place
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent(); // Use swapchain extent, not window, because could be larger than window

        // Initial values you want to clear the frame buffer to
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0}; // farthest is 1, 0 is closest

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Every frame, dynamicaly set the viewport and scissor just before submitting the buffer to be executed
        // Always set the right window size even if the swap chain changes
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, lveSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void LveRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() &&
               "Can't end render pass on command buffer from different frame");

        // End the render pass
        vkCmdEndRenderPass(commandBuffer);
    }
}
