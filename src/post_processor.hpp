#pragma once

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "shader.hpp"
#include "sprite_renderer.hpp"
#include "texture.hpp"

class PostProcessor {
public:
    PostProcessor(Shader& shader, size_t width, size_t height)
        : m_shader { shader }
        , m_width { width }
        , m_height { height }
        , m_confuse { false }
        , m_chaos { false }
        , m_shake { false }
    {
        // initialize renderbuffer/framebuffer object
        glGenFramebuffers(1, &m_multisampledFrameBufferObject);
        glGenFramebuffers(1, &m_frameBufferObject);
        glGenRenderbuffers(1, &m_renderBufferObject);

        // initialize renderbuffer storage with a multisampled color buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_multisampledFrameBufferObject);
        glBindRenderbuffer(GL_RENDERBUFFER, m_renderBufferObject);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_renderBufferObject);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::POSTPROCESSOR: Failed to initialize MSFBO" << std::endl;

        // initialize the FBO/texture to blit multisampeld color buffer
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferObject);
        m_texture.generate(width, height, nullptr);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.getID(), 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::POSTPROCESSOR: Failed to initialize FBO" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // initialize render data and uniforms
        initRenderData();
        m_shader.use();
        m_shader.setInt("scene", 0);
        float offset { 1.0f / 300.0f };
        float offsets[9][2] = {
            { -offset, offset }, // top left
            { 0.0f, offset }, // top center
            { offset, offset }, // top right
            { -offset, 0.0f }, // center left
            { 0.0f, 0.0f }, // center center
            { offset, 0.0f }, // center right
            { -offset, -offset }, // bottom left
            { 0.0f, -offset }, // bottom center
            { offset, -offset } // bottom right
        };
        glUniform2fv(glGetUniformLocation(m_shader.getID(), "offsets"), 9, (float*)offsets);
        int edgeKernel[9] = {
            -1, -1, -1,
            -1, 8, -1,
            -1, -1, -1
        };
        glUniform1iv(glGetUniformLocation(m_shader.getID(), "edgeKernel"), 9, edgeKernel);
        float blurKernel[9] = {
            1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f,
            2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f,
            1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f
        };
        glUniform1fv(glGetUniformLocation(m_shader.getID(), "blurKernel"), 9, blurKernel);
    }

    void beginRender()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_multisampledFrameBufferObject);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void endRender()
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_multisampledFrameBufferObject);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_frameBufferObject);
        glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void render(float time)
    {
        // set uniforms/options
        m_shader.use();
        m_shader.setFloat("time", time);
        m_shader.setInt("confuse", m_confuse);
        m_shader.setInt("chaos", m_chaos);
        m_shader.setInt("shake", m_shake);

        // render texture quad
        glActiveTexture(GL_TEXTURE0);
        m_texture.bind();
        glBindVertexArray(m_vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void setShake(bool isShaking) { m_shake = isShaking; }

private:
    Shader m_shader;
    Texture2D m_texture;
    size_t m_width, m_height;
    bool m_confuse, m_chaos, m_shake;
    GLuint m_multisampledFrameBufferObject, m_frameBufferObject, m_renderBufferObject, m_vertexArrayObject;

    void initRenderData()
    {
        // configure VAO/VBO
        GLuint vertexBufferObject;
        float vertices[] = {
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 0.0f, 1.0f,

            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &m_vertexArrayObject);
        glGenBuffers(1, &vertexBufferObject);

        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(m_vertexArrayObject);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};