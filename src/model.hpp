#pragma once

#define TINYOBJLOADER_IMPLEMENTATION

#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <algorithm>
#include <iostream>
#include <vector>

#include "mesh.hpp"
#include "shader.hpp"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 colors;
    glm::vec2 textureCoordinates;
};

class Model {
public:
    Model() { }

    void draw(Shader& shader)
    {
        for (auto& material : m_objReader.GetMaterials()) {
            shader.setFloat("material.shininess", material.shininess);
        }

        shader.setInt("material.diffuse", 0);
        glBindTexture(GL_TEXTURE_2D, m_diffuseTexture.id);

        shader.setInt("material.specular", 1);
        glBindTexture(GL_TEXTURE_2D, m_specularTexture.id);

        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(m_vertexArray);
        glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void load(const std::string& path, const std::string& directory)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;

        m_objReader.ParseFromFile(directory + path);
        std::string warn { m_objReader.Warning() };
        std::string err { m_objReader.Error() };

        if (!warn.empty())
            std::cout << warn << std::endl;
        if (!err.empty())
            std::cout << err << std::endl;
        if (!m_objReader.Valid())
            std::cout << "error" << std::endl;

        tinyobj::attrib_t attrib { m_objReader.GetAttrib() };
        std::vector<tinyobj::shape_t> shapes { m_objReader.GetShapes() };

        glm::vec3 minValues { attrib.vertices[0], attrib.vertices[1], attrib.vertices[2] };
        glm::vec3 maxValues { attrib.vertices[0], attrib.vertices[1], attrib.vertices[2] };
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex;

                vertex.position = { attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2] };

                if (!attrib.normals.empty()) {
                    vertex.normal = { attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2] };
                } else {
                    vertex.normal = { 0, 1, 0 };
                }

                if (!attrib.texcoords.empty()) {
                    vertex.textureCoordinates = { attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1] };
                }

                if (!attrib.colors.empty()) {
                    vertex.colors = { attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2] };
                }

                minValues.x = std::min(vertex.position.x, minValues.x);
                minValues.y = std::min(vertex.position.y, minValues.y);
                minValues.z = std::min(vertex.position.z, minValues.z);
                maxValues.x = std::max(vertex.position.x, maxValues.x);
                maxValues.y = std::max(vertex.position.y, maxValues.y);
                maxValues.z = std::max(vertex.position.z, maxValues.z);

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }

        m_indexCount = indices.size();
        m_radius = (maxValues - minValues) / 2.0f;

        loadTextures(directory);

        createBuffers(vertices, indices);
    }

private:
    // model data
    tinyobj::ObjReader m_objReader;
    std::vector<Texture> m_textures;
    GLuint m_vertexArray, m_vertexBuffer, m_elementBuffer, m_indexCount;
    glm::vec3 m_radius;
    Texture m_diffuseTexture, m_specularTexture;

    void loadTextures(const std::string& directory)
    {
        std::vector<tinyobj::material_t> materials { m_objReader.GetMaterials() };

        for (auto& material : materials) {
            m_diffuseTexture.id = textureFromFile(material.diffuse_texname, directory);
            m_diffuseTexture.type = "texture_diffuse";
            m_diffuseTexture.path = directory + material.diffuse_texname;

            m_specularTexture.id = textureFromFile(material.specular_texname, directory);
            m_specularTexture.type = "texture_specular";
            m_specularTexture.path = directory + material.specular_texname;
        }
    }

    void createBuffers(std::vector<Vertex> vertices, std::vector<GLuint> indices)
    {
        glGenVertexArrays(1, &m_vertexArray);
        glGenBuffers(1, &m_vertexBuffer);
        glGenBuffers(1, &m_elementBuffer);

        glBindVertexArray(m_vertexArray);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

        // vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // vertex normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        glEnableVertexAttribArray(1);

        // vertex colors
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, colors));
        glEnableVertexAttribArray(1);

        // vertex texture coords
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureCoordinates));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    unsigned int textureFromFile(const std::string& path, const std::string& directory)
    {
        std::string filename = directory + path;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format { GL_RED };

            if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cout << "Texture failed to load at path: " << filename << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};
