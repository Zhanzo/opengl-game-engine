#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "shader.hpp"

struct Texture {
    GLuint id;
    std::string type;
    std::string path;
};

