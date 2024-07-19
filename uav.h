#ifndef UAV
#define UAV

#include "glm/glm.hpp"

#include "gameObject.h"
#include "rigidbody.h"

class Uav : public GameObject, protected Rigidbody{
    public:
        using GameObject::position;

        int forceLimit = 50;
        float speedRate = 2;
        float slowRate = 20;

        Uav(const Mesh &mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        float m = 1, glm::vec3 currentVelocity = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 currentForce = glm::vec3(0.0f, 0.0f, 0.0f)
        ) : GameObject(mesh, position, up, front), Rigidbody(position, up, front, m, currentVelocity, currentForce){}

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool speedLock, float deltaTime);

    private:
        bool stable = false;
        float thrustForce = 0;

        inline float getLift(){
            return liftCoefficient * airDensity * (pow(veloctiy, 2) / 2) * wingArea;
        }

        inline float getDrag(){
            return dragCoefficient * airDensity * (pow(veloctiy, 2) / 2) * referenceArea;
        }

        float liftCoefficient = 0.4;
        float airDensity = 0.2;
        float wingArea = 20;

        float dragCoefficient = 0.4;
        float referenceArea = 25;

};

void Uav::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool speedLock, float deltaTime){
    float rollVelocity = 50 * deltaTime;
    float xAngle = 0, yAngle = 0, zAngle = 0;
    if (q)
        zAngle = -rollVelocity;
    if (e)
        zAngle = rollVelocity;
    if (left)
        xAngle = rollVelocity;
    if (right)
        xAngle = -rollVelocity;
    if (down)
        yAngle = -rollVelocity;
    if (up)
        yAngle = rollVelocity;
    
    if(q || e || right || left || up || down){
        glm::quat quaternion = rotate(xAngle, yAngle, zAngle);
        Rigidbody::front = GameObject::front;
        Rigidbody::up = GameObject::up;
        Rigidbody::right = GameObject::right;
        rotateForces(quaternion);
    }

    glm::vec3 newForce(0.0, 0.0, 0.0);

    if(speedUp){
        thrustForce += speedRate * deltaTime;

        //addForce(speedRate * deltaTime * GameObject::front);
        //std::cout << "Force: " << currentForce.x << " " << currentForce.y <<  " " << currentForce.z  << "\n";
    }
    if(slowDown){
        thrustForce -= slowRate * deltaTime;
        if(thrustForce < 0)
            thrustForce = 0;
        //addForce(slowRate * deltaTime * -GameObject::front);
        //std::cout << "Force: " << currentForce.x << " " << currentForce.y <<  " " << currentForce.z  << "\n";
    }

    if(speedLock){
        stable = !stable;
    }
    
    newForce += getLift() * GameObject::up;
    // weight
    newForce += getLift() * -GameObject::up;

    newForce += getDrag() * -GameObject::front;
    std::cout << "Drag: " << glm::length(getDrag()) << "\n";

    if(stable){
        thrustForce = getDrag();
    }
    newForce += thrustForce * GameObject::front;
    setForce(newForce);

    //std::cout << "Velocity: " << glm::length(currentVelocity) << "\n";
    std::cout << "Velocity: " << veloctiy << "\n";

    update(deltaTime);
    //if(glm::dot(glm::normalize(Rigidbody::front), glm::normalize(currentVelocity)) < 0)
    //    currentVelocity = glm::vec3(0,0,0);
    
    GameObject::position = Rigidbody::position;
    //std::cout << "Position: " << GameObject::position.x << " " << GameObject::position.y <<  " " << GameObject::position.z  << "\n";
    //std::cout << "Force magniuted: " << glm::length(currentForce) << "\n";
    //std::cout << "Velocity: " << currentVelocity.x << " " << currentVelocity.y <<  " " << currentVelocity.z  << "\n";
}

#endif