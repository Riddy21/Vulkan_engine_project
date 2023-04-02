#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"

namespace lve {
    class FirstApp {
        public:
            // Learning: constexpr is evaluated at compile time when possible
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();
        private:
            // Learning constructed here, that means that object will construct and deconstruct with the app
            LveWindow lveWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
            LveDevice lveDevice{lveWindow};
            LvePipeline lvePipeline{
                lveDevice,
                "shaders/simple_shader.vert.spv",
                "shaders/simple_shader.frag.spv",
                LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}
