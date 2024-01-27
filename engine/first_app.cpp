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

                environment.update(0.003f);// 60fps

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
        pong::Ball ball = pong::Ball{environment, {-0.6f,-2.0f}, 0.02f, red};
        pong::Ball ball2 = pong::Ball{environment, {-0.6f,-2.1f}, 0.02f, red};
        environment.addBall(std::make_shared<pong::Ball>(ball));
        environment.addBall(std::make_shared<pong::Ball>(ball2));

        // generate a bunch of walls in waterfall order
        pong::Wall wall1 = pong::Wall{environment, {{{-0.8f, -0.6f}, {-0.2f, -0.4f}}}, white};
        pong::Wall wall2 = pong::Wall{environment, {{{0.8f, -0.3f}, {0.2f, -0.1f}}}, white};
        pong::Wall wall3 = pong::Wall{environment, {{{-0.8f, 0.f}, {-0.2f, 0.2f}}}, white};
        pong::Wall wall4 = pong::Wall{environment, {{{0.8f, 0.3f}, {0.2f, 0.4f}}}, white};
        environment.addWall(std::make_shared<pong::Wall>(wall1));
        environment.addWall(std::make_shared<pong::Wall>(wall2));
        environment.addWall(std::make_shared<pong::Wall>(wall3));
        environment.addWall(std::make_shared<pong::Wall>(wall4));

    }
}
