#ifndef COLLIDER_H
#define COLLIDER_H

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"

enum ColliderType{
    STATIC,
    DYNAMIC,
    SPEACIAL
};

struct CollisionInfo{
    float penetration;
    glm::vec3 normal;
    glm::vec3 pointOnPlane;
};

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

        Collider(float* objectVertices, int size, ColliderType type = STATIC, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f)){ 
            this->type = type;
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
            //scale.x = abs(maxX - minX) * objectScale.x;
            //scale.y = abs(maxY - minY) * objectScale.y;
            //scale.z = abs(maxZ - minZ) * objectScale.z; 
            scale.x = abs(maxX - minX);
            scale.y = abs(maxY - minY);
            scale.z = abs(maxZ - minZ); 

            offset.x = (1 - scale.x) / 2;
            offset.y = (1 - scale.y) / 2;
            offset.z = (1 - scale.z) / 2;

            scale = objectScale;

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
        
        Collider(float* objectVertices, int size, glm::vec3 objectScale, ColliderType type, 
        glm::vec3 position, glm::vec3 up, glm::vec3 front, glm::vec3 right) : Collider(objectVertices, size, type, objectScale){
            this->position = position;
            this->up = up;
            this->front = front;
            this->right = right;
        }
        
        void draw(Shader &shader);
        bool hit(const Collider &other, CollisionInfo &collision);
        void onHit(const CollisionInfo& col);
        void update(glm::quat orientation);
        void set(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right);

    private:
        unsigned int VAO, VBO, EBO;
        static const float lineVertices[];
        ColliderType type;
        
        bool checkCollisionAxis(const Collider &other, glm::vec3 axis, CollisionInfo &colInfo);
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
        vertex += offset;
        vertex = scale * vertex;
        vertex = orientation * vertex;
        vertex += position;
        
        /*glm::vec4 vertex = glm::vec4(vertices[i], vertices[i+1], vertices[i+2], 1.0f);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = model * glm::toMat4(orientation);
        model = glm::scale(model, scale); 
        model = glm::translate(model, offset); 
        vertex = model * vertex;*/
        
        worldVertices[i] = vertex.x;
        worldVertices[i+1] = vertex.y;
        worldVertices[i+2] = vertex.z;
    }

    //std::cout << worldVertices[0] << " " << worldVertices[1] << " " << worldVertices[2] << "\n";
}

bool Collider::checkCollisionAxis(const Collider &other, glm::vec3 axis, CollisionInfo &colInfo){
    glm::vec3 min1Point(worldVertices[0], worldVertices[1], worldVertices[2]);
    glm::vec3 max1Point(worldVertices[0], worldVertices[1], worldVertices[2]);
    float min1 = glm::dot(axis, glm::vec3(worldVertices[0], worldVertices[1], worldVertices[2]));
    float max1 = glm::dot(axis, glm::vec3(worldVertices[0], worldVertices[1], worldVertices[2]));

    float min2 = glm::dot(axis, glm::vec3(other.worldVertices[0], other.worldVertices[1], other.worldVertices[2]));
    float max2 = glm::dot(axis, glm::vec3(other.worldVertices[0], other.worldVertices[1], other.worldVertices[2]));

    for(int i = 0; i < vertices.size(); i+=3){
        float p = glm::dot(axis, glm::vec3(worldVertices[i], worldVertices[i+1], worldVertices[i+2]));
        if (p < min1){
            min1Point = glm::vec3(worldVertices[i], worldVertices[i+1], worldVertices[i+2]);
            min1 = p;
        } 
        else if (p > max1) {
            max1 = p;
            max1Point = glm::vec3(worldVertices[i], worldVertices[i+1], worldVertices[i+2]);
        }
            
    }

    for(int i = 0; i < other.vertices.size(); i+=3){
        float p = glm::dot(axis, glm::vec3(other.worldVertices[i], other.worldVertices[i+1], other.worldVertices[i+2]));
        if (p < min2)
            min2 = p;
        else if (p > max2)
            max2 = p; 
    }


    if (min1 <= min2 && max1 >= min2){
        colInfo.normal = axis;
        colInfo.penetration = min2 - max1;
        colInfo.pointOnPlane = max1Point + colInfo.normal * colInfo.penetration;
        return true;
    }

    if (min2 <= min1 && max2 >= min1){
        colInfo.normal = axis;
        colInfo.penetration = min1 - max2;
        colInfo.pointOnPlane = min1Point + colInfo.normal * colInfo.penetration;
        return true;
    }

    return false;
}

bool Collider::hit(const Collider &other, CollisionInfo &collision){
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

    CollisionInfo bestCollisionInfo;
    CollisionInfo currentCollisionInfo;
    bestCollisionInfo.penetration = -FLT_MAX;

    for(int i = 0; i < axes1.size(); i++){
        if(!checkCollisionAxis(other, axes1[i], currentCollisionInfo))
            return false;
        if(currentCollisionInfo.penetration >= bestCollisionInfo.penetration)
            bestCollisionInfo = currentCollisionInfo;
    }

    for(int i = 0; i < axes2.size(); i++){
        if(!checkCollisionAxis(other, axes2[i], currentCollisionInfo))
            return false;
        if(currentCollisionInfo.penetration >= bestCollisionInfo.penetration)
            bestCollisionInfo = currentCollisionInfo;
    }

    collision = bestCollisionInfo;
    return true;
}

void Collider::onHit(const CollisionInfo& col){
    if(type == STATIC || type == SPEACIAL)
        return;

    glm::vec3 normalMag = col.normal * col.penetration;

    if(glm::length((position + normalMag) - col.pointOnPlane) > glm::length(position- col.pointOnPlane)){
        position += normalMag;
    }else{
        position -= normalMag;
    }
    //position += col.normal * col.penetration * 1.2f;
}

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

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

    glBindVertexArray(0);
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


#endif