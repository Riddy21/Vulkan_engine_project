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
        // flip the velocity in the y direction because of vulkan coordinate system
        transform2d.translation += (glm::vec2(velocity.x, -velocity.y) * delta_time) * environment.scale;
    }

    void Ball::bounce(float parallel) {
        // get the normal from the parallel
        float normal = parallel + glm::pi<float>() / 2.f;
        // rotate the velocity by the normal
        glm::mat2 rotation = glm::mat2(glm::cos(normal), glm::sin(normal), -glm::sin(normal), glm::cos(normal));
        velocity = rotation * velocity * 0.9f; // Apply a 10% energy loss
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

    void Environment::update(float delta_time) {
        for (auto ball : ballObjects) {
            for (auto wall : wallObjects) {
                if (detectCollisions(*ball, *wall)) {
                    ball->bounce(wall->transform2d.rotation);
                    continue;
                }
            }
            ball->update(delta_time);
        }
    }

    bool Environment::detectCollisions(const Ball &ball, const Wall &wall) {
        // Get the vertices of the wall
        std::vector<lve::LveModel::Vertex> wall_vertices = wall.model->getVertices();
        // Get the vertices of the ball
        std::vector<lve::LveModel::Vertex> ball_vertices = ball.model->getVertices();
        // transform and rotate on wall vertices to get them in the right spot, and add them to a vector
        for (auto &vertex : wall_vertices) {
            glm::mat2 rotation = glm::mat2(glm::cos(wall.transform2d.rotation), glm::sin(wall.transform2d.rotation),
                                           -glm::sin(wall.transform2d.rotation), glm::cos(wall.transform2d.rotation));
            glm::mat2 scale = glm::mat2(wall.transform2d.scale.x, 0.f, 0.f, wall.transform2d.scale.y);
            vertex.position = wall.transform2d.translation + (rotation * scale * vertex.position);
        }
        // transform on ball vertices to get them in the right spot, and add them to a vector
        for (auto &vertex : ball_vertices) {
            vertex.position = ball.transform2d.translation + (vertex.position * ball.transform2d.scale);
        }
        // Check if any of the ball vertices are inside the wall
        for (auto &ball_vertex : ball_vertices) {
            for (int i = 0; i < wall_vertices.size(); i += 3) {
                // Get the vertices of the triangle
                glm::vec3 vertex1 = glm::vec3(wall_vertices[i].position, 0.0f);
                glm::vec3 vertex2 = glm::vec3(wall_vertices[i + 1].position, 0.0f);
                glm::vec3 vertex3 = glm::vec3(wall_vertices[i + 2].position, 0.0f);
                // Check if the ball vertex is inside the triangle
                if (glm::abs(glm::cross(vertex2 - vertex1, vertex3 - vertex1)) ==
                    glm::abs(glm::cross(vertex2 - vertex1, glm::vec3(ball_vertex.position, 0.0f) - vertex1)) +
                    glm::abs(glm::cross(vertex3 - vertex2, glm::vec3(ball_vertex.position, 0.0f) - vertex2)) +
                    glm::abs(glm::cross(vertex1 - vertex3, glm::vec3(ball_vertex.position, 0.0f) - vertex3))) {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector<std::shared_ptr<lve::LveGameObject>> Environment::getGameObjects() const {
        std::vector<std::shared_ptr<lve::LveGameObject>> gameObjects;
        for (auto ball : ballObjects) {
            gameObjects.push_back(ball);
        }
        for (auto wall : wallObjects) {
            gameObjects.push_back(wall);
        }
        return gameObjects;
    }
}