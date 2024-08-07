#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "glm/glm.hpp"

class Rigidbody{ 
    public:
        glm::vec3 position;
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

        float m;
        float verticalVelocity = 0;
        float horizontalVelocity = 0;
        glm::vec3 currentForce;

        Rigidbody(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float m = 1){
            this->position = position;
            this->front = front;
            this->m = m;
            this->currentForce = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        Rigidbody(const Rigidbody& rg){
            this->position = rg.position;
            this->front = rg.front;
            this->m = rg.m;
        }
    
        Rigidbody& operator=(const Rigidbody& rg){
            this->position = rg.position;
            this->front = rg.front;
            this->m = rg.m;
            return *this;
        }

        inline void setForce(glm::vec3 force);
        inline void addForce(glm::vec3 force);
        void update(float deltaTime, bool directionBased);

        inline float getWeight(){
            return  m * gForce;
        }
    private:
        const float gForce = 9.8f;
};

inline void Rigidbody::setForce(glm::vec3 force){
    currentForce = force;
}

inline void Rigidbody::addForce(glm::vec3 force){
    currentForce += force;
}

void Rigidbody::update(float deltaTime, bool directionBased = true){
    glm::vec3 a = currentForce / m;

    float verticalAcceleration = (sqrt(pow(currentForce.x, 2) + pow(currentForce.z, 2)) / m);
    float newHVelocity = horizontalVelocity;

    if(front.z * currentForce.z > 0)
        newHVelocity += verticalAcceleration * deltaTime;
    else{
        newHVelocity -= verticalAcceleration * deltaTime;
        if(newHVelocity < 0.5f)
            newHVelocity = 0;
    }

    float xRatio = front.x == 0 ? 0 : front.x / (abs(front.x) + abs(front.z));
    float zRatio = front.z == 0 ? 0 : front.z / (abs(front.x) + abs(front.z));

    float avgVe = (horizontalVelocity + newHVelocity) * 0.5f;
    position.x += avgVe * deltaTime * xRatio;
    position.z += avgVe * deltaTime * zRatio;
    position.y += verticalVelocity * deltaTime;

    horizontalVelocity = newHVelocity;
    verticalVelocity += (currentForce.y / m) * deltaTime;;
}

#endif