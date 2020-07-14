#pragma once

#include "game_object.hpp"

const glm::vec2 SIZE { 60.0f, 20.0f };
const glm::vec2 VELOCITY { 0.0f, 150.0f };

class PowerUp : public GameObject {
public:
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject { position, SIZE, texture, color, VELOCITY }
        , m_type { type }
        , m_duration { duration }
        , m_isActivated { false }
    {
    }

    void setIsActivated(bool isActivated) { m_isActivated = isActivated; }

    void setDuration(float duration) { m_duration = duration; }

    bool getIsActivated() const { return m_isActivated; }

    float getDuration() const { return m_duration; }

    std::string getType() const { return m_type; }

private:
    std::string m_type;
    float m_duration;
    bool m_isActivated;
};