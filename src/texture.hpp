#pragma once

#include <glad/glad.h>

#include <stddef.h>

class Texture2D {
public:
    Texture2D()
        : m_width { 0 }
        , m_height { 0 }
        , m_internalFormat { GL_RGB }
        , m_imageFormat { GL_RGB }
        , m_wrapS { GL_REPEAT }
        , m_wrapT { GL_REPEAT }
        , m_filterMin { GL_LINEAR }
        , m_filterMax { GL_LINEAR }
    {
        glGenTextures(1, &m_id);
    }

    void generate(size_t width, size_t height, unsigned char* data)
    {
        m_width = width;
        m_height = height;

        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, m_imageFormat, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_filterMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_filterMax);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void deleteTexture()
    {
        glDeleteTextures(1, &m_id);
    }

    void bind() const { glBindTexture(GL_TEXTURE_2D, m_id); }

    GLuint getID() const { return m_id; }

    void setInternalFormat(GLuint format) { m_internalFormat = format; }

    void setImageFormat(GLuint format) { m_imageFormat = format; }

private:
    GLuint m_id;
    size_t m_width, m_height;
    GLuint m_internalFormat, m_imageFormat;
    GLuint m_wrapS, m_wrapT;
    GLuint m_filterMin, m_filterMax;
};
