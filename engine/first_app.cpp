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

                environment.update(0.1f);

                lveRenderer.beginSwapChainRenderPass(commandBuffer); // Record the command buffer, set up the render system
                simpleRenderSystem.renderGameObjects(commandBuffer, environment.getGameObjects());
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
        glm::vec3 red = {1.0f, .0f, .0f};
        pong::Ball ball = pong::Ball{environment, {-0.4f,0.5f}, 0.02f, red};
        pong::Wall wall = pong::Wall{environment, {{{-0.9f, -0.9f}, {0.9f, -0.7f}}}, white};
        environment.addBall(std::make_shared<pong::Ball>(ball));
        environment.addWall(std::make_shared<pong::Wall>(wall));
    }
}
