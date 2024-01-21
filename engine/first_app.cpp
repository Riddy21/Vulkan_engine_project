#include "first_app.hpp"

#include "simple_render_system.hpp"
#include "pong.hpp"

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

                lveRenderer.beginSwapChainRenderPass(commandBuffer); // Record the command buffer, set up the render system
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer); // Stop recording the command buffer
                lveRenderer.endFrame(); // Submits the command buffer
            }
        }
        // CPU will block until GPU operations are completed
        // When device is deleted, the command pool and buffer is destroyed as well
        vkDeviceWaitIdle(lveDevice.device());
    }

    void FirstApp::loadGameObjects(){
        glm::vec3 white = {1.0f, 1.0f, 1.0f};
        auto wall = pong::Wall{lveDevice, {{{0.0f,0.5f}, {0.0f, 1.0f}}}, white};

        gameObjects.push_back(wall.getGameObject()); // moves the ownership of the object
    }
}
