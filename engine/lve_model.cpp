#include "lve_model.hpp"

// std
#include <cassert>
#include <cstring>

namespace lve {
    LveModel::LveModel(LveDevice &device, const std::vector<Vertex> &vertices) : lveDevice{device} {
        createVertexBuffers(vertices);
    }

    LveModel::~LveModel() {
        vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
        // The number of memory allocations is limited
        // Allowed to manage memory seperately so you can do one allocation and use it many times
        vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
    }

    void LveModel::createVertexBuffers(const std::vector<Vertex> &vertices) {
        // Initialize teh memory of the buffer size
        vertexCount = static_cast<uint32_t>(vertices.size()); // static cast is the basic compile time cast in c++
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount; // total number of bytes to store the vertices of the model

        //
        lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, // combines the bit operators together
            // HOST_VISIBLE_BIT Want the allocated memory to be available to CPU, necessary to write to device memory
            // HOST_COHERENT_BIT Keeps host and device memory regions consistent with each toher  
            vertexBuffer,
            vertexBufferMemory
        );

        void * data;
        // Create a region of host memory mapped to device memory
        // and sets data to point to the beginning of the mapped meory range
        vkMapMemory(lveDevice.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize)); // All vertex data will be accounted for
        vkUnmapMemory(lveDevice.device(), vertexBufferMemory);

        // Memcpy
        // Takes the host (CPU) data and copies it to the host mapped memory region
        // Host coherent will flushed to the device memory region
    }

    void LveModel::draw(VkCommandBuffer commandBuffer) {
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    void LveModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer}; // Sets the buffers array as an array of vertex buffers, can add more later
        VkDeviceSize offsets[] = {0};

        // Binds the command buffer given to the vertex buffers that we offer
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
        // Corresponds to our vertex buffer, bound to location 0, strde advances by size of Vertex
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1); // Sets the size
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex); // Sets the number of buffer steps to skip, will auto map to our struct
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2); // Needs to match the number of output vertex attribues
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // The format of our data
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0; // inteleaving position and color in 1 binding
        attributeDescriptions[1].location = 1; // Must match with location in vertex shader
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT; // The format of our data
        attributeDescriptions[1].offset = offsetof(Vertex, color); // Will automatically calculate the byte offset of the color member in the vertex struct
                                                                   // Makes sure the order which the Vertex struct is declared doesn't matter
        return attributeDescriptions;
    }
}