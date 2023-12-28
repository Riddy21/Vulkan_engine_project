#pragma once

#include "lve_device.hpp"

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>

#include <vector>

namespace lve {
    // Take vertex data created by the CPU or read in a file,
    // Then allocate and copy the data to the device GPU to be 
    // rendered efficiently
    class LveModel {
        public:
            // Define a struct that wraps the glm vertext buffer
            struct Vertex {
                glm::vec2 position;
                glm::vec3 color; // interleaved with position in binding

                // Static funcitons
                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };
            //Delete the copy constructors, because Vulkan manages the memory
            LveModel(LveDevice &device, const std::vector<Vertex> &vertices);
            ~LveModel();

            LveModel(const LveModel &) = delete;
            LveModel &operator=(const LveModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &verticies);

            LveDevice& lveDevice;
            // Buffer and memory are seperate objects
            // Puts programmers in control of memory management
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
}