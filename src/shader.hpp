#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

enum Shaders { Vertex,
    Fragment,
    Geometry,
    Program };

class Shader {
public:
    Shader() { }

    void compile(const std::string& vSource, const std::string& fSource, const std::string& gSource = "")
    {
        // vertex shader
        const char* vertexSource { vSource.c_str() };
        GLuint vertexShader { glCreateShader(GL_VERTEX_SHADER) };
        glShaderSource(vertexShader, 1, &vertexSource, nullptr);
        glCompileShader(vertexShader);
        checkCompileErrors(vertexShader, Vertex);

        // fragment shader
        const char* fragmentSource { fSource.c_str() };
        GLuint fragmentShader { glCreateShader(GL_FRAGMENT_SHADER) };
        glShaderSource(fragmentShader, 1, &fragmentSource, nullptr);
        glCompileShader(fragmentShader);
        checkCompileErrors(fragmentShader, Fragment);

        // geometry shader
        GLuint geometryShader;
        if (gSource != "") {
            const char* geometrySource { gSource.c_str() };
            geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometryShader, 1, &geometrySource, nullptr);
            checkCompileErrors(geometryShader, Geometry);
        }

        // shader program
        m_id = glCreateProgram();
        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);
        if (gSource != "")
            glAttachShader(m_id, geometryShader);
        glLinkProgram(m_id);
        checkCompileErrors(m_id, Program);

        // delete the shaders
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        if (gSource != "")
            glDeleteShader(geometryShader);
    }

    void use() { glUseProgram(m_id); }

    void deleteShader() { glDeleteProgram(m_id); }

    void setBool(const std::string& name, bool value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform1i(location, (int)value);
    }

    void setInt(const std::string& name, int value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform1i(location, value);
    }

    void setFloat(const std::string& name, float value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform1f(location, value);
    }

    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void setVec2(const std::string& name, float x, float y) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform2f(location, x, y);
    }

    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void setVec3(const std::string& name, float x, float y, float z) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform3f(location, x, y, z);
    }

    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void setVec4(const std::string& name, float x, float y, float z, float w) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniform4f(location, x, y, z, w);
    }

    void setMat2(const std::string& name, const glm::mat2& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setMat3(const std::string& name, const glm::mat3& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void setMat4(const std::string& name, const glm::mat4& value) const
    {
        GLint location { glGetUniformLocation(m_id, name.c_str()) };
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

private:
    GLuint m_id;

    void checkCompileErrors(GLuint object, Shaders type)
    {
        int success;
        char infoLog[1024];

        if (type != Program) {
            glGetShaderiv(object, GL_COMPILE_STATUS, &success);

            if (!success) {
                glGetShaderInfoLog(object, 1024, nullptr, infoLog);
                std::cerr << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- "
                          << std::endl;
            }
        } else {
            glGetProgramiv(object, GL_LINK_STATUS, &success);

            if (!success) {
                glGetProgramInfoLog(object, 1024, NULL, infoLog);
                std::cout << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- "
                          << std::endl;
            }
        }
    }
};
