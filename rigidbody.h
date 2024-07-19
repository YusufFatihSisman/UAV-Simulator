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
        float veloctiy = 0;
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
        void update(float deltaTime, bool directionBased);
};

void Rigidbody::rotateForces(glm::quat quaternion){
    float force = glm::length(currentForce);
    float vc = glm::length(currentVelocity);

    if(force != 0){
        currentForce = glm::normalize(quaternion * glm::normalize(currentForce));
        currentForce *= force;
    }

    if(vc != 0){
        currentVelocity = glm::normalize(quaternion * glm::normalize(currentVelocity));
        currentVelocity *= vc;
    }
}

inline void Rigidbody::setForce(glm::vec3 force){
    currentForce = force;
}

inline void Rigidbody::addForce(glm::vec3 force){
    currentForce += force;
}

void Rigidbody::update(float deltaTime, bool directionBased = true){
    glm::vec3 a = currentForce / m;
    //position += currentVelocity * deltaTime;
    //currentVelocity += a * deltaTime;

    if(directionBased)
        position += veloctiy * deltaTime * front;
    else {
        position += currentVelocity * deltaTime;
        currentVelocity += a * deltaTime;
    }

    if(front.z * currentForce.z > 0)
        veloctiy += (glm::length(currentForce) / m) * deltaTime;
    else{
        veloctiy -= (glm::length(currentForce) / m) * deltaTime;
        if(veloctiy < 0.5f)
            veloctiy = 0;
    }
        

}

#endif