#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

#include "game_level.hpp"
#include "resource_manager.hpp"
#include "sprite_renderer.hpp"

enum GameState {
    Active,
    Menu,
    Win
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
        delete m_player;
        delete m_renderer;
    }

    void init(ResourceManager& resourceManager)
    {
        // load shaders
        resourceManager.loadShader("sprite", "shader.vert", "shader.frag");
        Shader shader { resourceManager.getShader("sprite") };

        // configure shaders
        glm::mat4 projection { glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f) };
        shader.use();
        shader.setInt("image", 0);
        shader.setMat4("projection", projection);

        m_renderer = new SpriteRenderer { shader };

        // load textures
        resourceManager.loadTexture("textures/background.jpg", false, "background");
        resourceManager.loadTexture("textures/awesomeface.jpg", true, "face");
        resourceManager.loadTexture("textures/block.png", false, "block");
        resourceManager.loadTexture("textures/block_solid.png", false, "block_solid");
        resourceManager.loadTexture("textures/paddle.png", true, "paddle");

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

        glm::vec2 playerPos { m_width / 2.0f - m_playerSize.x / 2.0f, m_height - m_playerSize.y };
        Texture2D playerTexture { resourceManager.getTexture("paddle") };
        m_player = new GameObject { playerPos, m_playerSize, playerTexture };
    }

    void processInput(float deltaTime)
    {
        if (m_state == Active) {
            float velocity { m_playerVelocity * deltaTime };

            if (m_keys[GLFW_KEY_A])
                m_player->setPositionX(glm::max(m_player->getPositionX() - velocity, 0.0f));
            if (m_keys[GLFW_KEY_D])
                m_player->setPositionX(glm::min(m_player->getPositionX() + velocity, m_width - m_player->getSizeX()));
        }
    }

    void update(float deltaTime) { }

    void render(ResourceManager& resourceManager)
    {
        if (m_state == Active) {
            Texture2D texture { resourceManager.getTexture("background") };
            m_renderer->drawSprite(texture, glm::vec2(0.0f, 0.0f), glm::vec2(m_width, m_height), 0.0f);

            m_levels[m_level].draw(*m_renderer);
            m_player->draw(*m_renderer);
        }
    }

    void setState(GameState state) { m_state = state; }

    void setKey(int key, bool isPressed) { m_keys[key] = isPressed; }

private:
    SpriteRenderer* m_renderer;
    GameObject* m_player;
    GameState m_state;
    std::vector<GameLevel> m_levels;
    std::vector<bool> m_keys;
    size_t m_width, m_height, m_level;
    const glm::vec2 m_playerSize { 100.0f, 20.0f };
    const float m_playerVelocity { 500.0f };
};
