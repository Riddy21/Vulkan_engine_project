#include "first_app.hpp"

namespace lve {
    void FirstApp::run() {
        // while the window does not want to close, poll window events
        while (!lveWindow.shouldClose()) {
            glfwPollEvents();
        }
    }
}
