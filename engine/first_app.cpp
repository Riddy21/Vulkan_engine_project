#include "first_app.hpp"

namespace lve {
    void FirstApp::run() {
        // while the window does not want to close, poll window events
        while (!lveWindow.shouldClose()) {
            // Poll window events. eg. Keystrokes and actions
            glfwPollEvents();
        }
    }
}
