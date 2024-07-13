#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "mesh.h"
class GameObject : public Mesh{
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;
        
        GameObject(const Mesh &mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f)) 
        : Mesh(mesh.vertices, mesh.indices, mesh.textures){
        
            this->position = position;
            this->up = up;
            this->front = front;
            right = glm::normalize(glm::cross(front, up));
        }

        glm::mat4 getRotationMatrix(){
            return glm::lookAt(position, position + front, up);
        }

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, float deltaTime);

        void rotate(float xAngle, float yAngle, float zAngle);
};

void GameObject::processInput(bool q, bool e, bool right, bool left, bool up, bool down, float deltaTime){
    float rollVelocity = 50 * deltaTime;
    float xAngle = 0, yAngle = 0, zAngle = 0;
    if (q)
        zAngle = rollVelocity;
    if (e)
        zAngle = -rollVelocity;
    if (left)
        xAngle = -rollVelocity;
    if (right)
        xAngle = +rollVelocity;
    if (down)
        yAngle = rollVelocity;
    if (up)
        yAngle = -rollVelocity;

    if(q || e || right || left || up || down)
        rotate(xAngle, yAngle, zAngle);
}

void GameObject::rotate(float xAngle, float yAngle, float zAngle){
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

    front = glm::normalize(qCombined * front);
    right = glm::normalize(qCombined * right);
    up    = glm::normalize(glm::cross(right, front)); 
}


#endif