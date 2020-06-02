#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stb_image.h>

#include <iostream>
#include <vector>

#include "mesh.h"
#include "shader.h"

class Model {
  public:
    Model(std::string path) { load_model(path); }

    void draw(Shader shader) {
        for (unsigned int i{0}; i < meshes.size(); ++i) meshes[i].draw(shader);
    }

  private:
    // model data
    std::vector<Texture> textures_loaded;
    std::vector<Mesh>    meshes;
    std::string          directory;

    void load_model(std::string path) {
        Assimp::Importer import;
        const aiScene*   scene{import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs)};

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR:ASSIMP::" << import.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_first_of('/'));

        process_node(scene->mRootNode, scene);
    }

    void process_node(aiNode* node, const aiScene* scene) {
        for (unsigned int i{0}; i < node->mNumMeshes; ++i) {
            aiMesh* mesh{scene->mMeshes[node->mMeshes[i]]};
            meshes.push_back(process_mesh(mesh, scene));
        }
        for (unsigned int i{0}; i < node->mNumChildren; ++i) {
            process_node(node->mChildren[i], scene);
        }
    }

    Mesh process_mesh(aiMesh* mesh, const aiScene* scene) {
        std::vector<Vertex>  vertices;
        std::vector<GLuint>  indices;
        std::vector<Texture> textures;

        for (unsigned int i{0}; i < mesh->mNumVertices; ++i) {
            Vertex vertex;

            // process vertex positions
            glm::vec3 position{mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            vertex.position = position;

            // process vertex normals
            glm::vec3 normal{mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
            vertex.normal = normal;

            // process vertex texture coords
            if (mesh->mTextureCoords[0]) {
                glm::vec2 tex_coords{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
                vertex.tex_coords = tex_coords;
            } else
                vertex.tex_coords = glm::vec2(0.0f, 0.0f);

            vertices.push_back(vertex);
        }

        for (unsigned int i{0}; i < mesh->mNumFaces; ++i) {
            aiFace face{mesh->mFaces[i]};
            for (unsigned int j{0}; j < face.mNumIndices; ++j) indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0) {
            aiMaterial*          material{scene->mMaterials[mesh->mMaterialIndex]};
            std::vector<Texture> diffuse_maps{
                load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse")};
            textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
            std::vector<Texture> specular_maps{
                load_material_textures(material, aiTextureType_SPECULAR, "texture_specular")};
            textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
        }

        return Mesh{vertices, indices, textures};
    }

    std::vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, std::string type_name) {
        std::vector<Texture> textures;
        for (unsigned int i{0}; i < mat->GetTextureCount(type); ++i) {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip{false};
            for (unsigned int j{0}; j < textures_loaded.size(); ++j) {
                if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                Texture texture;
                texture.id   = texture_from_file(str.C_Str(), directory);
                texture.type = type_name;
                texture.path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }
        return textures;
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