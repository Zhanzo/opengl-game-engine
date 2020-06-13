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

    void setIsDestroyed(float isDestroyed) { m_isDestroyed = isDestroyed; }

    void setPosition(glm::vec2 position) { m_position = position; }

    void setPositionX(float position) { m_position.x = position; }

    void setPositionY(float position) { m_position.y = position; }

    void setVelocity(glm::vec2 velocity) { m_velocity = velocity; }

    void setVelocityX(float velocity) { m_velocity.x = velocity; }

    void setVelocityY(float velocity) { m_velocity.y = velocity; }

    void setSize(glm::vec2 size) { m_size = size; }

    bool getIsSolid() { return m_isSolid; }

    bool getIsDestroyed() { return m_isDestroyed; }

    glm::vec2 getPosition() { return m_position; }

    float getPositionX() { return m_position.x; }

    float getPositionY() { return m_position.y; }

    glm::vec2 getVelocity() { return m_velocity; }

    float getVelocityX() { return m_velocity.x; }

    float getVelocityY() { return m_velocity.y; }

    glm::vec2 getSize() { return m_size; }

    float getSizeX() { return m_size.x; }

    float getSizeY() { return m_size.y; }

protected:
    glm::vec2 m_position, m_size, m_velocity;
    glm::vec3 m_color;
    Texture2D m_sprite;
    float m_rotation;
    bool m_isSolid, m_isDestroyed;
};
