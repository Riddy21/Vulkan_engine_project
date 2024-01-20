#include "first_app.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <stdexcept>

namespace lve {

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
        // while the window does not want to close, poll window events
        while (!lveWindow.shouldClose()) {
            // Poll window events. eg. Keystrokes and actions
            glfwPollEvents();

            if (auto commandBuffer = lveRenderer.beginFrame()) { // will return nullptr if swapchain needs to be recreated

                // begin offscreen shadow pass
                // render shadow casting objects
                //end offscreen shadow pass

                lveRenderer.beginSwapChainRenderPass(commandBuffer); // Record the command buffer
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer); // Stop recording the command buffer
                lveRenderer.endFrame();
            }
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
}
