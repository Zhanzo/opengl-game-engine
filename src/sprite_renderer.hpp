#pragma once

#include <glm/glm.hpp>

#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "shader.hpp"
#include "texture.hpp"

class SpriteRenderer {
public:
    SpriteRenderer(Shader& shader)
        : m_shader { shader }
    {
        initRenderData();
    }

    ~SpriteRenderer() { glDeleteVertexArrays(1, &m_quadVertexArray); }

    void drawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(10), float rotation = 0, glm::vec3 color = glm::vec3(1))
    {
        m_shader.use();
        glm::mat4 model { glm::mat4(1) };
        model = glm::translate(model, glm::vec3(position, 0));

        model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0));
        model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 0, 1));
        model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

        model = glm::scale(model, glm::vec3(size, 1));

        m_shader.setMat4("model", model);
        m_shader.setVec3("spriteColor", color);

        glActiveTexture(GL_TEXTURE0);
        texture.bind();

        glBindVertexArray(m_quadVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

private:
    Shader m_shader;
    GLuint m_quadVertexArray;

    void initRenderData()
    {
        GLuint vertexBuffer;
        float vertices[] = {
            0, 1, 0, 1,
            1, 0, 1, 0,
            0, 0, 0, 0,

            0, 1, 0, 1,
            1, 1, 1, 1,
            1, 0, 1, 0
        };

        glGenVertexArrays(1, &m_quadVertexArray);
        glGenBuffers(1, &vertexBuffer);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(m_quadVertexArray);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};
