#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.hpp"
#include "shader.hpp"
#include "texture.hpp"

struct Particle {
    glm::vec2 m_position, m_velocity;
    glm::vec4 m_color;
    float m_life;

    Particle()
        : m_position { 0.0f }
        , m_velocity { 0.0f }
        , m_color { 1.0f }
        , m_life { 0.0f }
    {
    }
};

class ParticleGenerator {
public:
    ParticleGenerator(Shader& shader, Texture2D& texture, size_t amount)
        : m_shader { shader }
        , m_texture { texture }
        , m_amount { amount }
        , m_lastUsedParticle { 0 }
    {
        init();
    }

    void update(float deltaTime, GameObject& object, size_t newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f))
    {
        for (size_t i { 0 }; i < newParticles; ++i) {
            size_t unusedParticle { firstUnusedParticle() };
            respawnParticle(m_particles[unusedParticle], object, offset);
        }

        for (size_t i { 0 }; i < m_amount; ++i) {
            Particle& p { m_particles[i] };
            p.m_life -= deltaTime;

            if (p.m_life > 0.0f) {
                p.m_position -= p.m_velocity * deltaTime;
                p.m_color.a -= deltaTime * 2.5f;
            }
        }
    }

    void draw()
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        m_shader.use();

        for (auto& particle : m_particles) {
            if (particle.m_life > 0.0f) {
                m_shader.setVec2("offset", particle.m_position);
                m_shader.setVec4("color", particle.m_color);
                m_texture.bind();
                glBindVertexArray(m_vertexArrayObject);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

private:
    std::vector<Particle> m_particles;
    Shader m_shader;
    Texture2D m_texture;
    size_t m_amount, m_lastUsedParticle;
    GLuint m_vertexArrayObject;

    void init()
    {
        GLuint vertexBufferObject;
        float particleQuad[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &m_vertexArrayObject);
        glGenBuffers(1, &vertexBufferObject);
        glBindVertexArray(m_vertexArrayObject);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindVertexArray(0);

        for (size_t i { 0 }; i < m_amount; ++i)
            m_particles.push_back(Particle {});
    }

    size_t firstUnusedParticle()
    {
        for (size_t i { m_lastUsedParticle }; i < m_amount; ++i) {
            if (m_particles[i].m_life <= 0.0f) {
                m_lastUsedParticle = i;
                return i;
            }
        }

        for (size_t i { 0 }; i < m_lastUsedParticle; ++i) {
            if (m_particles[i].m_life <= 0.0f) {
                m_lastUsedParticle = i;
                return i;
            }
        }

        m_lastUsedParticle = 0;
        return 0;
    }

    void respawnParticle(Particle& particle, GameObject& object, glm::vec2 offset = glm::vec2(0.0f, 0.0f))
    {
        float random { ((rand() % 100) - 50) / 10.0f };
        float randomColor { 0.5f + ((rand() % 100) / 100.0f) };

        particle.m_position = object.getPosition() + random + offset;
        particle.m_color = glm::vec4(randomColor, randomColor, randomColor, 1.0f);
        particle.m_life = 1.0f;
        particle.m_velocity = object.getVelocity() * 0.1f;
    }
};
