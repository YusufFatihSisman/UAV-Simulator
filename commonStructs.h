#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

#include "glm/glm.hpp"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

#endif