#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "mesh.h"

class GameObject : public Mesh{
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        glm::quat orientation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
        
        GameObject(const Mesh &mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f)) 
        : Mesh(mesh.vertices, mesh.indices, mesh.textures){
        
            this->position = position;
            this->up = up;
            this->front = front;
            right = glm::normalize(glm::cross(front, up));
        }

        glm::mat4 getRotationMatrix(){
            return glm::toMat4(orientation);
        }

        glm::quat rotate(float xAngle, float yAngle, float zAngle);
};


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
    right = glm::normalize(qCombined * right);
    up    = glm::normalize(glm::cross(right, front)); 

    return qCombined;
}


#endif