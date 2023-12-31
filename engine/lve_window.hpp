#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lve {

    class LveWindow {
        public:
            LveWindow(int w, int h, std::string name);
            ~LveWindow();

            // Ensures that no copy of LveWindow isn't accidentally created
            LveWindow(const LveWindow &) = delete;
            LveWindow &operator=(const LveWindow &) = delete;

            bool shouldClose() {return glfwWindowShouldClose(window);}

            VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};}

            void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

            bool wasWindowResized() { return framebufferResized; }
            void resetWindowResizedFlag() { framebufferResized = false; }

        private:
            static void framebufferResizedCallback(GLFWwindow *winddow, int width, int height);
            void initWindow();

            int width; // Is resizeable
            int height; // Is resizable
            bool framebufferResized = false;

            std::string windowName;
            GLFWwindow *window;
    };
}
