#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "mesh.h"
#include "collider.h"

class GameObject : public Mesh{
    public:
        glm::vec3 position;
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
        glm::vec3 scale;
        glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

        Collider* collider = NULL;
        
        GameObject(){}

        GameObject(const Mesh &mesh, glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f)) 
        : Mesh(mesh.vertices, mesh.indices, mesh.textures){
            this->scale = scale;
            this->position = position;
            rotate(rotation.x, rotation.y, rotation.z);
        }

        GameObject(const GameObject& gameObject) : Mesh(gameObject){
            this->position = gameObject.position;
            this->front = gameObject.front;
            this->up = gameObject.up;
            this->right = gameObject.right;
            this->scale = gameObject.scale;
            this->orientation = gameObject.orientation;
            this->collider = gameObject.collider;
        }
    
        GameObject& operator=(const GameObject& gameObject){
            Mesh::operator = (gameObject);
            this->position = gameObject.position;
            this->front = gameObject.front;
            this->up = gameObject.up;
            this->right = gameObject.right;
            this->scale = gameObject.scale;
            this->orientation = gameObject.orientation;
            this->collider = gameObject.collider;
            return *this;
        }

        glm::mat4 getRotationMatrix(){
            return glm::toMat4(orientation);
        }

        glm::quat rotate(float xAngle, float yAngle, float zAngle);

        bool hit(const GameObject &other, CollisionInfo &collision);

        inline void addCollider(float* objectVertices, int size, ColliderType type = STATIC);
};

bool GameObject::hit(const GameObject &other, CollisionInfo &collision){
    if(collider == NULL || other.collider == NULL)
        return false;

    glm::vec3 endPos = collider->position;
    collider->position = position + front * collider->scale;
    collider->update(orientation);
    while(glm::length(position - collider->position) < glm::length(position - endPos)){
        if(collider->hit(*(other.collider), collision)){
            collider->onHit(collision);
            this->position = collider->position;
            return true;
        }
        collider->position += front * collider->scale;
        collider->update(orientation);
    }

    collider->position = endPos;
    collider->update(orientation);
    if(collider->hit(*(other.collider), collision)){
        collider->onHit(collision);
        this->position = collider->position;
        return true;
    }
    
    return false;
}

inline void GameObject::addCollider(float* objectVertices, int size, ColliderType type){
    collider = new Collider(objectVertices, size, scale, type, position, up, front, right);
}

glm::quat GameObject::rotate(float xAngle, float yAngle, float zAngle){
    float cosX = cos(glm::radians(yAngle/2));
    float sinX = sin(glm::radians(yAngle/2));

    float cosY = cos(glm::radians(xAngle/2));
    float sinY = sin(glm::radians(xAngle/2));
            
    float cosZ = cos(glm::radians(zAngle/2));
    float sinZ = sin(glm::radians(zAngle/2));

    glm::quat qX = glm::quat(cosX, right.x * sinX, right.y * sinX, right.z * sinX);
    glm::quat qY = glm::quat(cosY, up.x * sinY, up.y * sinY, up.z * sinY);
    glm::quat qZ = glm::quat(cosZ, front.x * sinZ, front.y * sinZ, front.z * sinZ);

    glm::quat qCombined = qZ * qY * qX;
    qCombined = glm::normalize(qCombined);

    orientation = glm::normalize(qCombined * orientation);

    front = glm::normalize(qCombined * front);
    up = glm::normalize(qCombined * up);
    //right = glm::normalize(qCombined * right);
    //up    = glm::normalize(glm::cross(right, front)); 
    right = glm::normalize(glm::cross(front, up));

    return qCombined;
}


#endif