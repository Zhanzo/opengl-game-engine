#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "shader.h"

struct Texture {
    GLuint      id;
    std::string type;
    std::string path;
};

#endif