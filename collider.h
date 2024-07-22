#ifndef COLLIDER_H
#define COLLIDER_H

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"

class Collider{
    public:
        std::vector<float> vertices;
        glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
        //std::vector<unsigned int> indices;
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        Collider(float* objectVertices, int size, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f)){ 
            float minY, maxY, minX, maxX, minZ, maxZ;

            minX = objectVertices[0];
            maxX = objectVertices[0];
            minY = objectVertices[1];
            maxY = objectVertices[1];
            minZ = objectVertices[2];
            maxZ = objectVertices[2];

            for(int i = 8; i < size; i+=8){
                if(objectVertices[i] < minX)
                    minX = objectVertices[i];
                if(objectVertices[i] > maxX)
                    maxX = objectVertices[i];
                
                if(objectVertices[i+1] < minY)
                    minY = objectVertices[i+1];
                if(objectVertices[i+1] > maxY)
                    maxY = objectVertices[i+1];
                
                if(objectVertices[i+2] < minZ)
                    minZ = objectVertices[i+2];
                if(objectVertices[i+2] > maxZ)
                    maxZ = objectVertices[i+2];
            }
            scale.x = abs(maxX - minX) * objectScale.x;
            scale.y = abs(maxY - minY) * objectScale.y;
            scale.z = abs(maxZ - minZ) * objectScale.z; 

            offset.x = (1 - scale.x) / 2;
            offset.y = (1 - scale.y) / 2;
            offset.z = (1 - scale.z) / 2;

            for(int i = 0; i < 72; i+=3){
                vertices.push_back(lineVertices[i]);
                vertices.push_back(lineVertices[i + 1]);
                vertices.push_back(lineVertices[i + 2]);
            }

            setupCollider();
        }
        void Draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;
        static const float lineVertices[];

        void setupCollider();

};

const float Collider::lineVertices[] = {
    // arka
    -0.5f, -0.5f, -0.5f, // left bot back
    0.5f, -0.5f, -0.5f, // right bot back

    0.5f,  0.5f, -0.5f, // right up back
    -0.5f,  0.5f, -0.5f, // left up back

    -0.5f, -0.5f, -0.5f, // left bot back
    -0.5f,  0.5f, -0.5f, // l t b

    0.5f,  0.5f, -0.5f, // r t b
    0.5f, -0.5f, -0.5f, // r b b
    // ön
    -0.5f, -0.5f,  0.5f, // l b f
    0.5f, -0.5f,  0.5f, // r b f

    0.5f,  0.5f,  0.5f, // r t f
    -0.5f,  0.5f,  0.5f, // l t f 

    -0.5f,  0.5f,  0.5f, // l t f
    -0.5f, -0.5f,  0.5f, // l b f

    0.5f,  0.5f,  0.5f, // r t f
    0.5f, -0.5f,  0.5f, // r b f
    // sol
    -0.5f,  0.5f, -0.5f, // l t b
    -0.5f,  0.5f,  0.5f, // l t f

    -0.5f, -0.5f, -0.5f, // l b b
    -0.5f, -0.5f,  0.5f, // l b f
    // sağ
    0.5f,  0.5f,  0.5f, // r t f
    0.5f,  0.5f, -0.5f, // r t b

    0.5f, -0.5f, -0.5f, // r b b
    0.5f, -0.5f,  0.5f, // r b f
};

void Collider::Draw(Shader &shader){
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, vertices.size());
    glBindVertexArray(0);
}

void Collider::setupCollider(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);  

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
    //             &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glBindVertexArray(0);
}

#endif