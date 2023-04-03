#include "first_app.hpp"

#include <stdexcept>

namespace lve {

    FirstApp::FirstApp() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        // while the window does not want to close, poll window events
        while (!lveWindow.shouldClose()) {
            // Poll window events. eg. Keystrokes and actions
            glfwPollEvents();
            drawFrame();
        }
        // CPU will block until GPU operations are completed
        vkDeviceWaitIdle(lveDevice.device());
    }

    // Creates a pipeline layout with defaults set and assigns it to the pipelineLayout pointer
    void FirstApp::createPipelineLayout(){
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        // used to pass data other than vertex data to vertex shaders
        pipelineLayoutInfo.pSetLayouts = nullptr;
        // Push constants are way to efficiently send small amount of data to shaders 
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(lveDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline() {
        auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
        // Render pass describes the structure and format of frame buffer attachments and structure
        // Blueprint to tell the graphic pipeline what to expect when it is time to render
        pipelineConfig.renderPass = lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );
        
    }

    void FirstApp::createCommandBuffers() {
        // Depends on whether computer supports double or triple buffering
        commandBuffers.resize(lveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
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

        if (vkAllocateCommandBuffers(lveDevice.device(),
                                     &allocInfo,
                                     commandBuffers.data()) != VK_SUCCESS){
            throw std::runtime_error("failed to allocate command buffers!");
        }

        // Initialize each command buffer
        for (int i=0; i<commandBuffers.size(); i++){
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent(); // Use swapchain extent, not window, because could be larger

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            // No secondary command buffers will be use
            // Commands wil be embedded in the primary command buffer itself
                // No mixing allowed
            // If you want to use secondary command buffers
                // SECONDARY_COMMAND_BUFFERS
                // You can add command buffers to teh command buffer
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvePipeline->bind(commandBuffers[i]);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0); // 3 verticies and only 1 instance, aren't using any offsets

            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }
    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        // fetches index of frame rendered next
        // Handles CPU GPU sync for v-sync
        auto result = lveSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // submits the command buffer to the device graphics queue, handle CPU-GPU sync
        // Command buffer will be executed
        // Submit to the display at the appropriate time
        result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

    }
}
