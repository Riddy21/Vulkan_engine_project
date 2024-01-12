#include "first_app.hpp"

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace lve {

    // In implementation because it's temporary
    struct SimplePushConstantData {
        glm::mat2 transform{1.f}; // Default initialized to identity matrix, basically no change
        glm::vec2 offset;
        alignas(16) glm::vec3 color; // Alignment issue with glfw struct, needs to align by 4 bytes
    };

    FirstApp::FirstApp() {
        loadGameObjects();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
        // command buffer is automatically destroyed
    }

    void FirstApp::run() {
        // while the window does not want to close, poll window events
        while (!lveWindow.shouldClose()) {
            // Poll window events. eg. Keystrokes and actions
            glfwPollEvents();
            drawFrame();
        }
        // CPU will block until GPU operations are completed
        // When device is deleted, the command pool and buffer is destroyed as well
        vkDeviceWaitIdle(lveDevice.device());
    }

    std::vector<LveModel::Vertex> FirstApp::draw_triangles(std::vector<LveModel::Vertex> input, unsigned int depth){
        if (depth == 0){
            return input;
        } else {
            std::vector<LveModel::Vertex> output;
            std::vector<LveModel::Vertex> buffer;
            for (int i=0; i<input.size(); i++){
                buffer.push_back(input[i]);
                buffer.push_back({(input[i].position + input[(i+1)%input.size()].position) * 0.5f, input[(i+1)%input.size()].color});
                buffer.push_back({(input[i].position + input[(i+2)%input.size()].position) * 0.5f, input[(i+2)%input.size()].color});
                buffer = draw_triangles(buffer, depth - 1);
                output.insert(output.end(), buffer.begin(), buffer.end());
                buffer.clear();
            }
            return output;
        }
    }

    void FirstApp::loadGameObjects(){
        vertices = { // First bracket is the vector
            {{0.0f, -0.3f}, {1.0f, 0.0f, 0.0f}}, // Each vertex, GLM vect2 position member
            {{0.3f, 0.3f}, {0.0f, 1.0f, 0.0f}},
            {{-0.3f, 0.3f}, {0.0f, 0.0f, 1.0f}}
        };
        auto new_vertices = draw_triangles(vertices, 1);

        // Intialize the model
        auto lveModel = std::make_shared<LveModel>(lveDevice, new_vertices); // using shared object to assign one model to multiple game objects

        auto triangle = LveGameObject::createGameObject();
        triangle.model = lveModel;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = {2.f, .5f};
        triangle.transform2d.rotation = .25f * glm::two_pi<float>(); // Vulkan coord y goes from - to +, so rotation is opposite

        gameObjects.push_back(std::move(triangle)); // moves the ownership of the object
    }

    // Creates a pipeline layout with defaults set and assigns it to the pipelineLayout pointer
    void FirstApp::createPipelineLayout(){
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT; // Want fragment and vertex shader to access push constants
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        // used to pass data other than vertex data to vertex shaders
        pipelineLayoutInfo.pSetLayouts = nullptr;
        // Push constants are way to efficiently send small amount of data to shaders 
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(lveDevice.device(),
                                   &pipelineLayoutInfo,
                                   nullptr,
                                   &pipelineLayout) != VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline() {
        assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        // Render pass describes the structure and format of frame buffer attachments and structure
        // Blueprint to tell the graphic pipeline what to expect when it is time to render
        // Multiple subpasses can be used for post processing effects
        pipelineConfig.renderPass = lveSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>( // using smart pointers
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );
        
    }

    void FirstApp::recreateSwapChain() {
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

        createPipeline();
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

    void FirstApp::createCommandBuffers() {
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

    void FirstApp::freeCommandBuffers() {
        vkFreeCommandBuffers(
            lveDevice.device(),
            lveDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex){
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        // Begin recording a command buffer
        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS){
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        // First command to begin a render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lveSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lveSwapChain->getFrameBuffer(imageIndex); // writing to a specific frame buffer

        // Setup the render area, area where shaders loads and stores take place
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lveSwapChain->getSwapChainExtent(); // Use swapchain extent, not window, because could be larger than window

        // Initial values you want to clear the frame buffer to
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0}; // farthest is 1, 0 is closest

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        renderGameObjects(commandBuffers[imageIndex]);

        // End the render pass
        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
        lvePipeline->bind(commandBuffer);

        for (auto& obj: gameObjects){
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(commandBuffer,
                               pipelineLayout,
                               VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                               0, // offset
                               sizeof(SimplePushConstantData),
                               &push);

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer); 

        }
    }

    void FirstApp::drawFrame() {
        uint32_t imageIndex;
        // fetches index of frame rendered next
        // Handles CPU GPU sync for v-sync
        auto result = lveSwapChain->acquireNextImage(&imageIndex);

        // Detect if the window has been resized
        if (result == VK_ERROR_OUT_OF_DATE_KHR) {// Detect if surface changed in such way that it is incompatible with swap chain
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            // Need to handle suboptimal case in the future due to window resizing
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        // Can we do this only after the swap chain is changed?
        // record command buffer every frame
        recordCommandBuffer(imageIndex); // no longer done at startup? Isn't this super inefficient?

        // submits the command buffer to the device graphics queue, handle CPU-GPU sync
        // Command buffer will be executed
        // Submit to the display at the appropriate time
        result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        // Detect after command buffer if it has been resized
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            lveWindow.wasWindowResized()) {
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return ;
        }


        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

    }
}
