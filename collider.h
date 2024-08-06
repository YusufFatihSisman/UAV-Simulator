#ifndef COLLIDER_H
#define COLLIDER_H

#include <iostream>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.h"
#include "commonStructs.h"

enum ColliderType{
    STATIC,
    DYNAMIC,
    GROUND,
    SPEACIAL
};

struct CollisionInfo{
    float penetration;
    glm::vec3 normal;
    glm::vec3 pointOnPlane;
    ColliderType type;
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

        glm::vec3 minAABB;
        glm::vec3 maxAABB;

        ColliderType type;

        Collider(const vector<Vertex> &objectVertices, ColliderType type = STATIC, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f)){ 
            this->type = type;
            float minY, maxY, minX, maxX, minZ, maxZ;

            minX = objectVertices[0].Position.x;
            maxX = objectVertices[0].Position.x;
            minY = objectVertices[0].Position.y;
            maxY = objectVertices[0].Position.y;
            minZ = objectVertices[0].Position.z;
            maxZ = objectVertices[0].Position.z;

            for(int i = 0; i < objectVertices.size(); i++){
                if(objectVertices[i].Position.x < minX)
                    minX = objectVertices[i].Position.x;
                if(objectVertices[i].Position.x > maxX)
                    maxX = objectVertices[i].Position.x;
                
                if(objectVertices[i].Position.y < minY)
                    minY = objectVertices[i].Position.y;
                if(objectVertices[i].Position.y > maxY)
                    maxY = objectVertices[i].Position.y;
                
                if(objectVertices[i].Position.z < minZ)
                    minZ = objectVertices[i].Position.z;
                if(objectVertices[i].Position.z > maxZ)
                    maxZ = objectVertices[i].Position.z;
            }
            scale.x = abs(maxX - minX);
            scale.y = abs(maxY - minY);
            scale.z = abs(maxZ - minZ); 

            offset.x = (maxX + minX) / 2;
            offset.y = (maxY + minY) / 2;
            offset.z = (maxZ + minZ) / 2;

            scale *= objectScale;
            
            for(int i = 0; i < 72; i+=3){
                glm::vec3 vertex = glm::vec3(lineVertices[i], lineVertices[i+1], lineVertices[i+2]);
                
                vertex = scale * vertex;
                vertex += offset;
                
                vertices.push_back(vertex.x);
                vertices.push_back(vertex.y);
                vertices.push_back(vertex.z);

                worldVertices[i] = vertex.x;
                worldVertices[i+1] = vertex.y;
                worldVertices[i+1] = vertex.z;
            }

            setupCollider();
        }
        
        Collider(const vector<Vertex> &objectVertices, glm::vec3 objectScale, ColliderType type, 
        glm::vec3 position, glm::vec3 up, glm::vec3 front, glm::vec3 right) : Collider(objectVertices, type, objectScale){
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
        bool checkBoundingBoxAlignedAxis(const Collider &other);

    private:
        unsigned int VAO, VBO, EBO;
        static const float lineVertices[];
        
        
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

    minAABB = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
    maxAABB = glm::vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for(int i = 0; i < vertices.size(); i += 3){
        glm::vec3 vertex = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        //vertex += offset;
        //vertex = scale * vertex;
        vertex = orientation * vertex;
        vertex += position;
        
        worldVertices[i] = vertex.x;
        worldVertices[i+1] = vertex.y;
        worldVertices[i+2] = vertex.z;

        if(vertex.x < minAABB.x)
            minAABB.x = vertex.x;
        if(vertex.y < minAABB.y)
            minAABB.y = vertex.y;
        if(vertex.z < minAABB.z)
            minAABB.z = vertex.z;
        
        if(vertex.x > maxAABB.x)
            maxAABB.x = vertex.x;
        if(vertex.y > maxAABB.y)
            maxAABB.y = vertex.y;
        if(vertex.z > maxAABB.z)
            maxAABB.z = vertex.z;
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
        colInfo.normal = -axis;
        colInfo.penetration = min1 - max2;
        colInfo.pointOnPlane = min1Point + colInfo.normal * colInfo.penetration;
        return true;
    }

    return false;
}

bool Collider::checkBoundingBoxAlignedAxis(const Collider &other){
    return (
        minAABB.x <= other.maxAABB.x &&
        maxAABB.x >= other.minAABB.x &&
        minAABB.y <= other.maxAABB.y &&
        maxAABB.y >= other.minAABB.y &&
        minAABB.z <= other.maxAABB.z &&
        maxAABB.z >= other.minAABB.z);
}

bool Collider::hit(const Collider &other, CollisionInfo &collision){
    if(!checkBoundingBoxAlignedAxis(other))
        return false;
    
    //std::cout << "Bounding box Collided\n";
    
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

    float size2 = axes2.size();

    for(int i = 0; i < 3; i++){
        for(int j = 0; j < size2; j++){
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
    collision.type = other.type;
    return true;
}

void Collider::onHit(const CollisionInfo& col){
    if(type == STATIC || type == SPEACIAL)
        return;

    if(col.type == SPEACIAL)
        return;

    position += col.normal * col.penetration;
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