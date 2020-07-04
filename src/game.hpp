#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

#include "ball_object.hpp"
#include "game_level.hpp"
#include "game_object.hpp"
#include "particle_generator.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"

enum GameState {
    Active,
    Menu,
    Win
};

enum Direction {
    Up,
    Right,
    Down,
    Left
};

struct Collision {
    bool hasCollided;
    Direction direction;
    glm::vec2 difference;
};

class Game {
public:
    Game(size_t width, size_t height)
        : m_state { Active }
        , m_keys(1024)
        , m_width { width }
        , m_height { height }
    {
    }

    ~Game()
    {
        delete m_renderer;
        delete m_player;
        delete m_ball;
        delete m_particles;
    }

    void init(ResourceManager& resourceManager)
    {
        // load shaders
        resourceManager.loadShader("sprite", "shader.vert", "shader.frag");
        resourceManager.loadShader("particle", "particle.vert", "particle.frag");

        // configure shader
        Shader shader { resourceManager.getShader("sprite") };
        glm::mat4 projection { glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f) };
        shader.use();
        shader.setInt("image", 0);
        shader.setMat4("projection", projection);

        // configure particle shader
        Shader particleShader { resourceManager.getShader("particle") };
        particleShader.use();
        particleShader.setMat4("projection", projection);

        // load textures
        resourceManager.loadTexture("textures/background.jpg", false, "background");
        resourceManager.loadTexture("textures/awesomeface.png", true, "face");
        resourceManager.loadTexture("textures/block.png", false, "block");
        resourceManager.loadTexture("textures/block_solid.png", false, "block_solid");
        resourceManager.loadTexture("textures/paddle.png", true, "paddle");
        resourceManager.loadTexture("textures/particle.png", true, "particle");

        Texture2D particleTexture { resourceManager.getTexture("particle") };
        m_renderer = new SpriteRenderer { shader };
        m_particles = new ParticleGenerator { particleShader, particleTexture, 500 };

        // load levels
        GameLevel levelOne, levelTwo, levelThree, levelFour;
        levelOne.load(resourceManager, "levels/one.lvl", m_width, m_height / 2);
        levelTwo.load(resourceManager, "levels/two.lvl", m_width, m_height / 2);
        levelThree.load(resourceManager, "levels/three.lvl", m_width, m_height / 2);
        levelFour.load(resourceManager, "levels/four.lvl", m_width, m_height / 2);

        m_levels.push_back(levelOne);
        m_levels.push_back(levelTwo);
        m_levels.push_back(levelThree);
        m_levels.push_back(levelFour);
        m_level = 0;

        // initialize player
        glm::vec2 playerPos { m_width / 2.0f - m_playerSize.x / 2.0f, m_height - m_playerSize.y };
        Texture2D playerTexture { resourceManager.getTexture("paddle") };
        m_player = new GameObject { playerPos, m_playerSize, playerTexture };

        // initialize ball
        glm::vec2 ballPos { playerPos + glm::vec2(m_playerSize.x / 2.0f - m_ballRadius, -m_ballRadius * 2.0f) };
        Texture2D ballTexture { resourceManager.getTexture("face") };
        m_ball = new BallObject { ballPos, m_ballRadius, m_initialBallVelocity, ballTexture };
    }

    void update(float deltaTime, ResourceManager& resourceManager)
    {
        m_ball->move(deltaTime, m_width);

        doCollisions();

        m_particles->update(deltaTime, *m_ball, 2, glm::vec2(m_ball->getRadius() / 2.0f));

        if (m_ball->getPositionY() >= m_height) {
            resetLevel(resourceManager);
            resetPlayer();
        }
    }

    void render(ResourceManager& resourceManager)
    {
        if (m_state == Active) {
            Texture2D texture { resourceManager.getTexture("background") };
            m_renderer->drawSprite(texture, glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);

            m_levels[m_level].draw(*m_renderer);
            m_player->draw(*m_renderer);
            m_particles->draw();
            m_ball->draw(*m_renderer);
        }
    }

    void resetLevel(ResourceManager& resourceManager)
    {
        switch (m_level) {
        case 0:
            m_levels[0].load(resourceManager, "levels/one.lvl", m_width, m_height / 2);
            break;
        case 1:
            m_levels[1].load(resourceManager, "levels/two.lvl", m_width, m_height / 2);
            break;
        case 2:
            m_levels[2].load(resourceManager, "levels/three.lvl", m_width, m_height / 2);
            break;
        case 3:
            m_levels[3].load(resourceManager, "levels/four.lvl", m_width, m_height / 2);
            break;
        }
    }

    void resetPlayer()
    {
        m_player->setSize(m_playerSize);
        m_player->setPosition(glm::vec2(m_width / 2.0f - m_player->getSizeX() / 2.0f, m_height - m_player->getSizeY()));
        m_ball->reset(m_player->getPosition() + glm::vec2(m_player->getSizeX() / 2.0f - m_ballRadius, -(m_ballRadius * 2.0f)), m_initialBallVelocity);
    }

    void processInput(float deltaTime)
    {
        if (m_state == Active) {
            float velocity { m_playerVelocity * deltaTime };
            float playerPositionX { m_player->getPositionX() };
            float ballPositionX { m_ball->getPositionX() };

            if (m_keys[GLFW_KEY_A]) {
                if (playerPositionX >= 0.0f) {
                    m_player->setPositionX(playerPositionX - velocity);
                    m_ball->setPositionX(m_ball->getIsStuck() ? ballPositionX - velocity : ballPositionX);
                }
            }

            if (m_keys[GLFW_KEY_D]) {
                if (playerPositionX <= m_width - m_player->getSizeX()) {
                    m_player->setPositionX(playerPositionX + velocity);
                    m_ball->setPositionX(m_ball->getIsStuck() ? ballPositionX + velocity : ballPositionX);
                }
            }

            if (m_keys[GLFW_KEY_SPACE])
                m_ball->setIsStuck(false);
        }
    }

    Direction vectorDirection(glm::vec2 target)
    {
        float max { 0.0f };
        unsigned int bestMatch { 0 };
        glm::vec2 compass[] {
            glm::vec2(0.0f, 1.0f), // up
            glm::vec2(1.0f, 0.0f), // right
            glm::vec2(0.0f, -1.0f), // down
            glm::vec2(-1.0f, 0.0f) // left
        };

        for (unsigned int i { 0 }; i < 4; ++i) {
            float dotProduct { glm::dot(glm::normalize(target), compass[i]) };

            if (dotProduct > max) {
                max = dotProduct;
                bestMatch = i;
            }
        }

        return (Direction)bestMatch;
    }

    // AABB – AABB collision
    bool checkCollision(GameObject& one, GameObject& two)
    {
        // collision x-axis?
        bool collisionX { one.getPositionX() + one.getSizeX() >= two.getPositionX() && two.getPositionX() + two.getSizeX() >= one.getPositionX() };
        // collision y-axis?
        bool collisionY { one.getPositionY() + one.getSizeY() >= two.getPositionY() && two.getPositionY() + two.getSizeY() >= one.getPositionY() };

        // collision only if objects collide on both axes
        return collisionX && collisionY;
    }

    // Circle – AABB collision
    Collision checkCollision(BallObject& one, GameObject& two)
    {
        // get center point of circle
        glm::vec2 center { one.getPosition() + one.getRadius() };

        // calculate AABB info (center, half-extents)
        glm::vec2 aabbHalfExtents { two.getSize() / 2.0f };
        glm::vec2 aabbCenter { two.getPosition() + aabbHalfExtents };

        // get difference vector between both centers
        glm::vec2 difference { center - aabbCenter };
        glm::vec2 clamped { glm::clamp(difference, -aabbHalfExtents, aabbHalfExtents) };

        // add clamped value to AABB center and we get the value of the box closest to the circle
        glm::vec2 closest { aabbCenter + clamped };

        // retrieve vector between center circle and closest point AABB and check if length <= radius
        difference = closest - center;

        if (glm::length(difference) <= one.getRadius())
            return Collision { true, vectorDirection(difference), difference };
        else
            return Collision { false, Up, glm::vec2(0.0f, 0.0f) };
    }

    void doCollisions()
    {
        auto bricks { m_levels[m_level].getBricks() };

        for (auto& box : *bricks) {
            if (!box.getIsDestroyed()) {
                Collision collision { checkCollision(*m_ball, box) };

                if (collision.hasCollided) {
                    if (!box.getIsSolid())
                        box.setIsDestroyed(true);

                    if (collision.direction == Left || collision.direction == Right) {
                        m_ball->setVelocityX(-m_ball->getVelocityX());

                        // relocate
                        float penetration { m_ball->getRadius() - std::abs(collision.difference.x) };

                        if (collision.direction == Left)
                            m_ball->setPositionX(m_ball->getPositionX() + penetration);
                        else
                            m_ball->setPositionX(m_ball->getPositionX() - penetration);
                    } else {
                        m_ball->setVelocityY(-m_ball->getVelocityY());

                        // relocate
                        float penetration { m_ball->getRadius() - std::abs(collision.difference.y) };

                        if (collision.direction == Up)
                            m_ball->setPositionY(m_ball->getPositionY() + penetration);
                        else
                            m_ball->setPositionY(m_ball->getPositionY() - penetration);
                    }
                }
            }
        }

        Collision collision { checkCollision(*m_ball, *m_player) };

        if (!m_ball->getIsStuck() && collision.hasCollided) {
            // check where it hit the board, and change the velocity
            float centerBoard { m_player->getPositionX() + m_player->getSizeX() / 2.0f };
            float distance { (m_ball->getPositionX() + m_ball->getRadius()) - centerBoard };
            float percentage { distance / (m_player->getSizeX() / 2.0f) };

            // then move accordingly
            float strength { 2.0f };
            glm::vec2 oldVelocity { m_ball->getVelocity() };
            m_ball->setVelocityX(m_initialBallVelocity.x * percentage * strength);
            m_ball->setVelocityY(-1.0f * std::abs(m_ball->getVelocityY()));
            m_ball->setVelocity(glm::normalize(m_ball->getVelocity()) * glm::length(oldVelocity));
        }
    }

    void setState(GameState state) { m_state = state; }

    void setKey(int key, bool isPressed) { m_keys[key] = isPressed; }

private:
    SpriteRenderer* m_renderer;
    ParticleGenerator* m_particles;
    GameObject* m_player;
    BallObject* m_ball;
    GameState m_state;
    std::vector<GameLevel> m_levels;
    std::vector<bool> m_keys;
    size_t m_width, m_height, m_level;
    const glm::vec2 m_playerSize { 100.0f, 20.0f };
    const glm::vec2 m_initialBallVelocity { 100.0f, -350.0f };
    const float m_playerVelocity { 500.0f };
    const float m_ballRadius { 12.5f };
};
