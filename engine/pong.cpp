#include "pong.hpp"

namespace pong {
    Wall::Wall(const Environment &environment, std::array<glm::vec2, 2> location, glm::vec3 color) :
            environment(environment),
            LveGameObject(lve::LveGameObject::createGameObject()) {
        // Create the wall model
        this->model = createWallModel();
        this->color = color;

        // scale to the right length
        float length = glm::distance(location[0], location[1]);
        transform2d.scale = {length, 1.f};
        // rotate to the right angle
        float angle = glm::atan(location[1].y - location[0].y, location[1].x - location[0].x);
        transform2d.rotation = angle;
        // translate to the right spot
        glm::vec2 center = (location[0] + location[1]) / 2.f;    
        transform2d.translation = center;
    }

    std::unique_ptr<lve::LveModel> Wall::createWallModel() {
        // Create the veritices for the wall and apply the color in a standard position
        std::vector<lve::LveModel::Vertex> model_vertices = {{{0.5f, thickness*0.5f}},
                                                             {{0.5f, -thickness*0.5f}},
                                                             {{-0.5f, thickness*0.5f}},
                                                             {{-0.5f, thickness*0.5f}},
                                                             {{-0.5f, -thickness*0.5f}},
                                                             {{0.5f, -thickness*0.5f}}};

        return std::make_unique<lve::LveModel>(environment.device, model_vertices);
    }

    Ball::Ball(const Environment &environment, glm::vec2 location, float size, glm::vec3 color) :
            environment(environment),
            position(location),
            radius(size),
            LveGameObject(lve::LveGameObject::createGameObject()) {
        // Create the ball model
        this->model = createBallModel();
        this->color = color;

        // scale to the right size
        transform2d.scale = {size, size};
        // translate to the right spot
        transform2d.translation = position;
    }

    void Ball::update(float delta_time) {
        velocity += environment.gravity * delta_time;
        // Update the position of the ball
        transform2d.translation += velocity * delta_time;
    }

    std::unique_ptr<lve::LveModel> Ball::createBallModel() {
        // For loop iterate through a circle and create a triangle fan
        std::vector<lve::LveModel::Vertex> model_vertices;
        for (int i = 0; i < 360; i++) {
            float angle = glm::radians(static_cast<float>(i));
            float next_angle = glm::radians(static_cast<float>(i + 1));
            glm::vec2 point1 = {glm::cos(angle), glm::sin(angle)};
            glm::vec2 point2 = {glm::cos(next_angle), glm::sin(next_angle)};
            model_vertices.push_back({{0.f, 0.f}});
            model_vertices.push_back({{point1.x, point1.y}});
            model_vertices.push_back({{point2.x, point2.y}});
        }
        return std::make_unique<lve::LveModel>(environment.device, model_vertices);
    }
}