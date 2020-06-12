#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "sprite_renderer.hpp"
#include "texture.hpp"

class GameObject {
public:
    GameObject()
        : m_position { 0.0f, 0.0f }
        , m_size { 1.0f, 1.0f }
        , m_velocity { 0.0f, 0.0f }
        , m_color { 1.0f, 1.0f, 1.0f }
        , m_rotation { 0.0f }
        , m_isSolid { false }
        , m_isDestroyed { false }
    {
    }

    GameObject(glm::vec2 pos, glm::vec2 size, Texture2D& sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f))
        : m_position { pos }
        , m_size { size }
        , m_velocity { velocity }
        , m_color { color }
        , m_sprite { sprite }
        , m_rotation { 0.0f }
        , m_isSolid { false }
        , m_isDestroyed { false }
    {
    }

    void draw(SpriteRenderer& renderer)
    {
        renderer.drawSprite(m_sprite, m_position, m_size, m_rotation, m_color);
    }

    void setIsSolid(bool isSolid) { m_isSolid = isSolid; }

    void setPositionX(float position) { m_position.x = position; }

    bool getIsSolid() { return m_isSolid; }

    bool getIsDestroyed() { return m_isDestroyed; }

    float getPositionX() { return m_position.x; }

    float getSizeX() { return m_size.x; }

protected:
    glm::vec2 m_position, m_size, m_velocity;
    glm::vec3 m_color;
    Texture2D m_sprite;
    float m_rotation;
    bool m_isSolid, m_isDestroyed;
};
