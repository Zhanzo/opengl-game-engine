#pragma once

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <vector>

#include "resource_manager.hpp"
#include "sprite_renderer.hpp"

enum GameState {
    Active,
    Menu,
    Win
};

class Game {
public:
    Game(unsigned int width, unsigned int height)
        : m_keys(1024)
        , m_width { width }
        , m_height { height }
    {
    }

    ~Game() { }

    void init(ResourceManager& resourceManager)
    {
        resourceManager.loadShader("sprite", "shader.vert", "shader.frag");

        glm::mat4 projection { glm::ortho(0.0f, static_cast<float>(m_width), static_cast<float>(m_height), 0.0f, -1.0f, 1.0f) };

        Shader shader { resourceManager.getShader("sprite") };
        shader.use();
        shader.setInt("image", 0);
        shader.setMat4("projection", projection);

        m_renderer = new SpriteRenderer(shader);
        resourceManager.loadTexture("awesomeface.png", true, "face");
    }

    void processInput(float deltaTime) { }

    void update(float deltaTime) { }

    void render(ResourceManager& resourceManager)
    {
        Texture2D texture { resourceManager.getTexture("face") };
        m_renderer->drawSprite(texture, glm::vec2(200), glm::vec2(300, 400), 45.0f, glm::vec3(0, 1, 0));
    }

    void setState(GameState state) { m_state = state; }

    void setKey(int key, bool isPressed) { m_keys[key] = isPressed; }

private:
    SpriteRenderer* m_renderer;
    GameState m_state { Active };
    std::vector<bool> m_keys;
    unsigned int m_width, m_height;
};
