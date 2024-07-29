#ifndef UAV
#define UAV

#include "glm/glm.hpp"

#include "gameObject.h"
#include "rigidbody.h"

class Uav : public GameObject, protected Rigidbody{
    public:
        using GameObject::position;

        int forceLimit = 50;
        float speedRate = 50;
        float slowRate = 50;

        Uav(){}

        Uav(const Mesh &mesh, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), float m = 1
        ) : GameObject(mesh, scale, position, rotation), Rigidbody(position, m){}

        Uav(const Uav& uav) : GameObject(uav), Rigidbody(uav){
            Rigidbody::front = GameObject::front;
        }
    
        Uav& operator=(const Uav& uav){
            GameObject::operator = (uav);
            Rigidbody::operator = (uav);
            Rigidbody::front = GameObject::front;
            return *this;
        }

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool speedLock, float deltaTime, vector<GameObject> &gameObjects);
        void onHit(const CollisionInfo &colInfo);

        void printInfo(){
            std::cout << "Drag: " << getDrag() << "\n";
            std::cout << "Thrust: " << thrustForce << "\n";
            std::cout << "Lift: " << getLift() << "\n";
            std::cout << "Weight: " << getWeight() << "\n";
            std::cout << "Velocity: " << vVelocity << "\n";
            std::cout << "Horizontal Velocity: " << hVelocity << "\n";
            std::cout << "Position: " << GameObject::position.x << " " << GameObject::position.y <<  " " << GameObject::position.z  << "\n";
        }

    private:
        bool stable = false;
        float thrustForce = 0;

        inline float getLift(){
            return liftCoefficient * airDensity * (pow(vVelocity, 2) / 2) * wingArea;
        }

        inline float getDrag(){
            return dragCoefficient * airDensity * (pow(vVelocity, 2) / 2) * referenceArea;
        }

        float liftCoefficient = 0.001;
        float airDensity = 0.2;
        float wingArea = 20;

        float dragCoefficient = 0.02;
        float referenceArea = 25;

};

void Uav::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool speedLock, float deltaTime, vector<GameObject> &gameObjects){
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
        rotateForces(quaternion);
    }

    glm::vec3 newForce(0.0, 0.0, 0.0);

    if(speedUp){
        thrustForce += speedRate * deltaTime;
    }
    if(slowDown){
        thrustForce -= slowRate * deltaTime;
        if(thrustForce < 0)
            thrustForce = 0;
    }

    if(speedLock){
        stable = !stable;
    }
    
    newForce += getLift() * GameObject::up;
    newForce += getWeight() * glm::vec3(0.0f, -1.0f, 0.0f);
    newForce += getDrag() * -GameObject::front;

    if(stable){
        thrustForce = getDrag();
        std::cout << "Lock\n";
    }
    newForce += thrustForce * GameObject::front;
    setForce(newForce);
    update(deltaTime);

    //GameObject::position = Rigidbody::position;
    if(collider != NULL){
        collider->set(Rigidbody::position, GameObject::front, GameObject::up, GameObject::right);
        collider->update(orientation);
        CollisionInfo colInfo;
        for(unsigned int i = 0; i < gameObjects.size(); i++){
            if(hit(gameObjects[i], colInfo))
                onHit(colInfo);
        }
        GameObject::position = collider->position;
        Rigidbody::position = GameObject::position;
    }else{
        GameObject::position = Rigidbody::position;
    }
}

void Uav::onHit(const CollisionInfo &colInfo){
    if(colInfo.type == GROUND)
        hVelocity = 0;    
    if(colInfo.type == STATIC){
        thrustForce = 0;
        vVelocity = 0;
    }  
}

#endif