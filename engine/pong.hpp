#pragma once

#include "lve_game_object.hpp"
#include "lve_model.hpp"

#include <array>

#define GLM_FORCE_RADIANS // Radians must be radians
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Expect depth values to 0 - 1
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace pong {
    class Environment;
    class Wall : public lve::LveGameObject{
        public:
        Wall(const Environment& environment, std::array<glm::vec2, 2> location, glm::vec3 color);

        private:
        std::unique_ptr<lve::LveModel> createWallModel();

        const Environment& environment;
        float thickness = 0.02f;
    };

    class Ball : public lve::LveGameObject{
        public:
        Ball(const Environment& environment, glm::vec2 location, float size, glm::vec3 colour);

        void update(float delta_time);
        void bounce(float normal);
        void changeVelocity(glm::vec2 new_velocity) {velocity = new_velocity;}

        private:
        std::unique_ptr<lve::LveModel> createBallModel();

        const Environment& environment;
        glm::vec2 position;
        float radius;
        glm::vec2 velocity{1.f, -1.f};
    };

    class Environment{
        public:
        Environment(lve::LveDevice &device, const glm::vec2 gravity) : device{device}, gravity{gravity} {};
        
        void addBall(std::shared_ptr<pong::Ball> ball) {ballObjects.push_back(ball);}
        void addWall(std::shared_ptr<pong::Wall> wall) {wallObjects.push_back(wall);}
        void update(float delta_time);
        std::vector<std::shared_ptr<lve::LveGameObject>> getGameObjects() const;

        lve::LveDevice& device;
        const glm::vec2 gravity;
        const float scale{0.1f};

        private:
        static bool detectCollisions(const Ball& ball, const Wall& wall);

        std::vector<std::shared_ptr<pong::Ball>> ballObjects;
        std::vector<std::shared_ptr<pong::Wall>> wallObjects;
    };

}