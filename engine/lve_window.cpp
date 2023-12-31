#include "lve_window.hpp"

#include <stdexcept>

namespace lve {
    // Learning: things that go after colon are argument intializations
    LveWindow::LveWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
        initWindow();
    }

    LveWindow::~LveWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void LveWindow::initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // Can't just resize
        // Because LVE swapchain encapsulates all attachments with fixed size
        // When windows size changed, need a new swap chain and pipeline
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this); // Pairs a GLFW object with a arbitrary pointer
        glfwSetFramebufferSizeCallback(window, framebufferResizedCallback); // Function will be called every time it's resized
    }

    void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface){
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS){
            throw std::runtime_error("failed to create window surface");
        }
    }

    void LveWindow::framebufferResizedCallback(GLFWwindow *window, int width, int height) {
        auto lveWindow = reinterpret_cast<LveWindow *>(glfwGetWindowUserPointer(window));
        lveWindow->framebufferResized = true;
        lveWindow->width = width;
        lveWindow->height = height;
    }
}
