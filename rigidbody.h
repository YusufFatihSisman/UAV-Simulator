#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "glm/glm.hpp"

class Rigidbody{ 
    public:
        glm::vec3 position;
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;

        float m;
        glm::vec3 currentVelocity;
        glm::vec3 currentForce;

        Rigidbody(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        float m = 1,  glm::vec3 currentVelocity = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 currentForce = glm::vec3(0.0f, 0.0f, 0.0f)){
            this->position = position;
            this->up = up;
            this->front = front;
            this->m = m;
            this->currentVelocity = currentVelocity;
            this->currentForce = currentForce;
            right = glm::normalize(glm::cross(front, up));
        }
        void rotateForces(glm::quat quaternion);
        inline void setForce(glm::vec3 force);
        inline void addForce(glm::vec3 force);
        void update(float deltaTime);
};

void Rigidbody::rotateForces(glm::quat quaternion){
    float force = glm::length(currentForce);
    float velocity = glm::length(currentVelocity);

    if(force != 0){
        currentForce = glm::normalize(quaternion * glm::normalize(currentForce));
        currentForce *= force;
    }

    if(velocity != 0){
        currentVelocity = glm::normalize(quaternion * glm::normalize(currentVelocity));
         currentVelocity *= velocity;
    }
}

inline void Rigidbody::setForce(glm::vec3 force){
    currentForce = force;
}

inline void Rigidbody::addForce(glm::vec3 force){
    currentForce += force;
}

void Rigidbody::update(float deltaTime){
    glm::vec3 a = currentForce / m;
    position += currentVelocity * deltaTime;
    currentVelocity += a * deltaTime;
    if(glm::dot(glm::normalize(front), glm::normalize(currentVelocity)) < 0)
        currentVelocity = glm::vec3(0,0,0);
}

#endif