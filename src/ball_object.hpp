#pragma once

#include "game_object.hpp"

class BallObject : public GameObject {
public:
    BallObject()
        : GameObject {}
        , m_radius { 12.5f }
        , m_isStuck { true }
    {
    }

    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, Texture2D& sprite)
        : GameObject { pos, glm::vec2(radius * 2.0f), sprite, glm::vec3(1.0f), velocity }
        , m_radius { radius }
        , m_isStuck { true }
    {
    }

    glm::vec2 move(float deltaTime, size_t windowWidth)
    {
        if (!m_isStuck) {
            m_position += m_velocity * deltaTime;

            if (m_position.x <= 0.0f) {
                m_velocity.x = -m_velocity.x;
                m_position.x = 0.0f;
            } else if (m_position.x + m_size.x >= windowWidth) {
                m_velocity.x = -m_velocity.x;
                m_position.x = windowWidth - m_size.x;
            }

            if (m_position.y <= 0.0f) {
                m_velocity.y = -m_velocity.y;
                m_position.y = 0.0f;
            }
        }

        return m_position;
    }

    void reset(glm::vec2 position, glm::vec2 velocity)
    {
        m_position = position;
        m_velocity = velocity;
        m_isStuck = true;
    }

    void setIsStuck(bool isStuck) { m_isStuck = isStuck; }

    float getRadius() { return m_radius; }

    bool getIsStuck() { return m_isStuck; }

private:
    float m_radius;
    bool m_isStuck;
};
