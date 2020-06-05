#ifndef MODEL_H
#define MODEL_H
#define TINYOBJLOADER_IMPLEMENTATION

#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <iostream>
#include <algorithm>
#include <vector>

#include "mesh.h"
#include "shader.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 colors;
    glm::vec2 tex_coords;
};

class Model {
  public:
    Model() { }

    void draw(Shader shader) {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void load(const std::string& path, const std::string& directory) {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        

        obj_reader.ParseFromFile(directory + path);
        std::string warn{obj_reader.Warning()};
        std::string err{obj_reader.Error()};

        if (!warn.empty()) std::cout << warn << std::endl;
        if (!err.empty()) std::cout << err << std::endl;
        if (!obj_reader.Valid()) std::cout << "error" << std::endl;

        tinyobj::attrib_t                attrib{obj_reader.GetAttrib()};
        std::vector<tinyobj::shape_t>    shapes{obj_reader.GetShapes()};

        glm::vec3 min_values{attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]};
        glm::vec3 max_values{attrib.vertices[0], attrib.vertices[1], attrib.vertices[2]};
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex;

                vertex.position = {attrib.vertices[3 * index.vertex_index + 0],
                                   attrib.vertices[3 * index.vertex_index + 1],
                                   attrib.vertices[3 * index.vertex_index + 2]};


                if (!attrib.normals.empty()) {
                    vertex.normal = {attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                    };
                } else {
                    vertex.normal = {0, 1, 0};
                }

                if (!attrib.texcoords.empty()) {
                    vertex.tex_coords = {attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]};
                }

                if (!attrib.colors.empty()) {
                    vertex.colors = {attrib.colors[3 * index.vertex_index + 0], attrib.colors[3 * index.vertex_index + 1],
                       attrib.colors[3 * index.vertex_index + 2]};
                }

                min_values.x = std::min(vertex.position.x, min_values.x);
                min_values.y = std::min(vertex.position.y, min_values.y);
                min_values.z = std::min(vertex.position.z, min_values.z);
                max_values.x = std::max(vertex.position.x, min_values.x);
                max_values.y = std::max(vertex.position.y, min_values.y);
                max_values.z = std::max(vertex.position.z, min_values.z);

                vertices.push_back(vertex);
                indices.push_back(indices.size());
            }
        }
        index_count = indices.size();
        radius = (max_values - min_values) / 2.0f;

        load_textures(directory);

        create_buffers(vertices, indices);
    }

  private:
    // model data
    tinyobj::ObjReader obj_reader;
    std::vector<Texture> textures;
    GLuint VAO, VBO, EBO, index_count;
    glm::vec3            radius;

    void load_textures(const std::string& directory) {
        std::vector<tinyobj::material_t> materials{obj_reader.GetMaterials()};

        for (auto& material : materials) {
            Texture texture_diffuse;
            texture_diffuse.id = texture_from_file(material.diffuse_texname.c_str(), directory);
            texture_diffuse.type = "texture_diffuse";
            texture_diffuse.path = directory + material.diffuse_texname;
            textures.push_back(texture_diffuse);

            Texture texture_specular;
            texture_specular.id = texture_from_file(material.specular_texname.c_str(), directory);
            texture_specular.type = "texture_ambient";
            texture_specular.path = directory + material.specular_texname;
            textures.push_back(texture_specular);
        }
    }

    void create_buffers(std::vector<Vertex> vertices, std::vector<GLuint> indices) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 &indices[0], GL_STATIC_DRAW);

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
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));
        glEnableVertexAttribArray(3);

        glBindVertexArray(0);
    }

    unsigned int texture_from_file(const char* path, const std::string& directory) {
        std::string filename = std::string(path);
        filename             = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int            width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
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
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
};

#endif