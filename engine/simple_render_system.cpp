#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace lve {

    struct SimplePushConstantData {
        glm::float32 rotate{0.0f}; // 4 bytes
        char pad[4]; // 4 bytes
        glm::vec2 scale{1.f, 1.f}; // 8 bytes
        glm::vec2 offset{.0f, 0.f}; // 8 bytes
        alignas(16) glm::vec3 color; // 12 bytes
    };

    SimpleRenderSystem::SimpleRenderSystem(LveDevice &device, VkRenderPass renderPass) : lveDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
        // command buffer is automatically destroyed
    }

    // Creates a pipeline layout with defaults set and assigns it to the pipelineLayout pointer
    void SimpleRenderSystem::createPipelineLayout(){
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

    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass) {
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        // Render pass describes the structure and format of frame buffer attachments and structure
        // Blueprint to tell the graphic pipeline what to expect when it is time to render
        // Multiple subpasses can be used for post processing effects
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>( // using smart pointers
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig
        );
        
    }

    void SimpleRenderSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects) {
        lvePipeline->bind(commandBuffer);

        for (auto& obj: gameObjects){
            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.rotate = obj.transform2d.rotation;
            push.scale = obj.transform2d.scale;

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
}
