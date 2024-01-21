#include "pong.hpp"

namespace pong {
    Wall::Wall(lve::LveDevice &device, std::array<glm::vec2, 2> location, glm::vec3 color) :
            device(device), wallGameObject(lve::LveGameObject::createGameObject()) {
        // Create the wall model
        wallGameObject.model = createWallModel();
        wallGameObject.color = color;

        // scale to the right length
        wallGameObject.transform2d.scale = {1.f, 2.f};
        // rotate to the right angle
        wallGameObject.transform2d.rotation = glm::pi<float>() / 2.f;
        // translate to the right spot
        wallGameObject.transform2d.translation = {0.5f, 0.5f};
    }

    std::unique_ptr<lve::LveModel> Wall::createWallModel() {
        // Create the veritices for the wall and apply the color in a standard position
        std::vector<lve::LveModel::Vertex> model_vertices = {{{0.01f, 0.01f}},
                                                             {{0.01f, -0.01f}},
                                                             {{-0.01f, 0.01f}},
                                                             {{-0.01f, 0.01f}},
                                                             {{-0.01f, -0.01f}},
                                                             {{0.01f, -0.01f}}};

        return std::make_unique<lve::LveModel>(device, model_vertices);
    }
}