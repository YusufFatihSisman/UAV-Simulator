#ifndef UAV
#define UAV

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

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, float deltaTime);

    private:
        float liftCoefficient;
        float airDensity;
        float wingArea;

        float dragCoefficient;
        float referenceArea;

};

void Uav::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, float deltaTime){
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

    if(speedUp){
        addForce(speedRate * deltaTime * GameObject::front);
        std::cout << "Force: " << currentForce.x << " " << currentForce.y <<  " " << currentForce.z  << "\n";
    }
    if(slowDown){
        addForce(slowRate * deltaTime * -GameObject::front);
         std::cout << "Force: " << currentForce.x << " " << currentForce.y <<  " " << currentForce.z  << "\n";
    }

    update(deltaTime);
    GameObject::position = Rigidbody::position;
    //std::cout << "Position: " << GameObject::position.x << " " << GameObject::position.y <<  " " << GameObject::position.z  << "\n";
    //std::cout << "Force magniuted: " << glm::length(currentForce) << "\n";
    //std::cout << "Velocity: " << currentVelocity.x << " " << currentVelocity.y <<  " " << currentVelocity.z  << "\n";
}

#endif