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
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        //std::vector<unsigned int> indices;

        Collider(float* vts, int verticeSize){ 
            for(int i = 0; i < verticeSize; i+=3){
                vertices.push_back(vts[i]);
                vertices.push_back(vts[i + 1]);
                vertices.push_back(vts[i + 2]);
            }

            setupCollider();
        }
        void Draw(Shader &shader);

    private:
        unsigned int VAO, VBO, EBO;

        void setupCollider();

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