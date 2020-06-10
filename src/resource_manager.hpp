#pragma once

#define STB_IMAGE_IMPLEMENTATION

#include <fstream>
#include <map>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <stb_image.h>

#include "shader.hpp"
#include "texture.hpp"

class ResourceManager {
public:
    ResourceManager() { }

    Shader loadShader(const std::string& name, const std::string& vertexShaderFile, const std::string& fragmentShaderFile, const std::string& geometryShaderFile = "")
    {
        m_shaders[name] = loadShaderFromFile(vertexShaderFile, fragmentShaderFile, geometryShaderFile);
        return m_shaders[name];
    }

    Shader getShader(const std::string& name)
    {
        return m_shaders[name];
    }

    Texture2D loadTexture(const std::string& file, bool hasAlpha, const std::string& name)
    {
        m_textures[name] = loadTextureFromFile(file, hasAlpha);
        return m_textures[name];
    }

    Texture2D getTexture(const std::string& name)
    {
        return m_textures[name];
    }

    void clear()
    {
        for (auto& it : m_shaders)
            it.second.deleteShader();
        for (auto& it : m_textures)
            it.second.deleteTexture();
    }

private:
    std::map<std::string, Shader> m_shaders;
    std::map<std::string, Texture2D> m_textures;

    Shader loadShaderFromFile(const std::string& vShaderFile, const std::string& fShaderFile, const std::string& gShaderFile)
    {
        std::string vertexCode, fragmentCode, geometryCode;

        try {
            std::ifstream vertexShaderFile { vShaderFile };
            std::ifstream fragmentShaderFile { fShaderFile };
            std::stringstream vertexShaderStream, fragmentShaderStream;

            vertexShaderStream << vertexShaderFile.rdbuf();
            fragmentShaderStream << fragmentShaderFile.rdbuf();

            vertexShaderFile.close();
            fragmentShaderFile.close();

            vertexCode = vertexShaderStream.str();
            fragmentCode = fragmentShaderStream.str();

            if (gShaderFile != "") {
                std::ifstream geometryShaderFile(gShaderFile);
                std::stringstream geometryShaderStream;
                geometryShaderStream << geometryShaderFile.rdbuf();
                geometryShaderFile.close();
                geometryCode = geometryShaderStream.str();
            }
        } catch (std::exception e) {
            std::cerr << "ERROR::SHADER::Failed to read shader files" << std::endl;
        }

        Shader shader;
        shader.compile(vertexCode, fragmentCode, geometryCode);
        return shader;
    }

    Texture2D loadTextureFromFile(const std::string& file, bool hasAlpha)
    {
        Texture2D texture;

        if (hasAlpha) {
            texture.setInternalFormat(GL_RGBA);
            texture.setImageFormat(GL_RGBA);
        }

        // laod image
        int width, height, nrChannels;
        unsigned char* data { stbi_load(file.c_str(), &width, &height, &nrChannels, 0) };
        texture.generate(width, height, data);
        stbi_image_free(data);
        return texture;
    }
};
