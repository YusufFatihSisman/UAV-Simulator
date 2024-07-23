#ifndef COLLIDER_H
#define COLLIDER_H

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"

class Collider{
    public:
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 offset = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

        std::vector<float> vertices;
        float worldVertices[72];

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

                worldVertices[i] = lineVertices[i];
                worldVertices[i+1] = lineVertices[i + 1];
                worldVertices[i+1] = lineVertices[i + 2];
            }

            setupCollider();
        }
        void draw(Shader &shader);
        bool hit(const Collider &other);
        void onHit();
        glm::vec2 getProject(glm::vec3 axis) const;
        void update(glm::quat orientation);
        void set(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right);

    private:
        unsigned int VAO, VBO, EBO;
        static const float lineVertices[];
        
        void setupCollider();
        

};

void Collider::set(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right){
    this->position = position;
    this->front = front;
    this->up = up;
    this->right = right;
}

void Collider::update(glm::quat orientation){

    for(int i = 0; i < vertices.size(); i += 3){
        glm::vec3 vertex = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        vertex = scale * vertex;
        vertex += offset;
        vertex = orientation * vertex;
        vertex += position;
        
        /*
        glm::vec4 vertex = glm::vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::toMat4(orientation);
        model = glm::translate(model, offset); 
        model = glm::scale(model, scale); 
        vertex = model * vertex;
        */
        worldVertices[i] = vertex.x;
        worldVertices[i+1] = vertex.y;
        worldVertices[i+2] = vertex.z;
    }

    //std::cout << worldVertices[0] << " " << worldVertices[1] << " " << worldVertices[2] << "\n";
}

glm::vec2 Collider::getProject(glm::vec3 axis) const{
    float min = glm::dot(axis, glm::vec3(worldVertices[0], worldVertices[1], worldVertices[2]));
    float max = glm::dot(axis, glm::vec3(worldVertices[0], worldVertices[1], worldVertices[2]));

    for(int i = 0; i < vertices.size(); i+=3){
        float p = glm::dot(axis, glm::vec3(worldVertices[i], worldVertices[i+1], worldVertices[i+2]));

        if (p < min) 
            min = p;
        else if (p > max) 
            max = p;
    }
    return glm::vec2(min, max);
}

bool Collider::hit(const Collider &other){

    vector<glm::vec3> axes1;
    vector<glm::vec3> axes2;
    axes1.push_back(front);
    axes1.push_back(right);
    axes1.push_back(up);

    bool exist1 = false, exist2 = false, exist3 = false;
    for(int i = 0; i < axes1.size(); i++){
        if(axes1[i] == other.front)
            exist1 = true;
        if(axes1[i] == other.right)
            exist2 = true;
        if(axes1[i] == other.up)
            exist3 = true;
    }
    if(!exist1)
        axes2.push_back(other.front);
    if(!exist2)
        axes2.push_back(other.right);
    if(!exist3)
        axes2.push_back(other.up);


    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++){
            bool exist = false;
            glm::vec3 newAxis = glm::normalize(glm::cross(axes1[i], axes2[j]));

            for(int k = 0; k < axes1.size(); k++){
                if(axes1[k] == newAxis){
                    exist = true;
                    break;
                }
            }
            if(exist)
                continue;

            for(int k = 0; k < axes2.size(); k++){
                if(axes2[k] == newAxis){
                    exist = true;
                    break;
                }
            }
            if(exist)
                continue;
            if(newAxis.x != 0 && newAxis.y != 0 && newAxis.z != 0)
                axes1.push_back(newAxis);
        }
    }

    for(int i = 0; i < axes1.size(); i++){
        glm::vec2 line1 = getProject(axes1[i]);
        glm::vec2 line2 = other.getProject(axes1[i]);
        if(line1.y < line2.x || line2.y < line1.x)
            return false;
    }
    for(int i = 0; i < axes2.size(); i++){
        glm::vec2 line1 = getProject(axes2[i]);
        glm::vec2 line2 = other.getProject(axes2[i]);
        if(line1.y < line2.x || line2.y < line1.x)
            return false;
    }
    return true;
}

void onHit(){
    std::cout << "Hit";
}

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

void Collider::draw(Shader &shader){
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