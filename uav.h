#ifndef UAV
#define UAV

#include "glm/glm.hpp"

#include "gameObject.h"
#include "rigidbody.h"

class Uav : public GameObject, protected Rigidbody{
    public:
        using GameObject::position;

        int forceLimit = 50;
        float speedRate = 200;
        float slowRate = 200;

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

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool cruiseMod, float deltaTime, vector<GameObject> &gameObjects);
        void onHit(const CollisionInfo &colInfo);
        glm::quat bankingTurn(float xAngle);

        void printInfo(){
            //std::cout << "Yaw : " << yaw << "\n";
            //std::cout << "Pitch : " << pitch << "\n";
            //std::cout << "Roll : " << roll << "\n";
            std::cout << "Drag: " << getDrag() << "\n";
            std::cout << "Thrust: " << thrustForce << "\n";
            std::cout << "Lift: " << getLift() << "\n";
            std::cout << "Weight: " << getWeight() << "\n";
            std::cout << "Velocity: " << vVelocity << "\n";
            std::cout << "Horizontal Velocity: " << hVelocity << "\n";
            std::cout << "Position: " << GameObject::position.x << " " << GameObject::position.y <<  " " << GameObject::position.z  << "\n";
            /*std::cout << "Up: " << GameObject::up.x << " " << GameObject::up.y <<  " " << GameObject::up.z  << "\n";
            std::cout << "Front: " << GameObject::front.x << " " << GameObject::front.y <<  " " << GameObject::front.z  << "\n";
            std::cout << "Right: " << GameObject::right.x << " " << GameObject::right.y <<  " " << GameObject::right.z  << "\n";*/
        }

    private:
        bool triggerLock = false;
        bool cruiseFlight = false;
        float thrustForce = 0;

        inline float getLift(){
            return (cruiseFlight && pitch == 0) ? getWeight() : liftCoefficient * airDensity * (pow(vVelocity, 2) / 2) * wingArea * glm::sin(glm::radians(pitch));
        }

        inline float getDrag(){
            return dragCoefficient * airDensity * (pow(vVelocity, 2) / 2) * referenceArea;
        }

        float liftCoefficient = 0.01;
        float airDensity = 0.2;
        float wingArea = 20;

        float dragCoefficient = 0.005;
        float referenceArea = 25;

        //float hVelocity = 0;
        //float vVelocity = 0;

};

void Uav::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, bool slowDown, bool cruiseMod, float deltaTime, vector<GameObject> &gameObjects){
    float rotationVelocity = 50 * deltaTime;
    float xAngle = 0, yAngle = 0, zAngle = 0;
    if (q)
        zAngle = (roll > -30) ? -rotationVelocity : 0;
    if (e)
        zAngle = (roll < 30) ? rotationVelocity : 0;
    if(!q && !e){
        if(roll < 1 && roll > -1)
            rotationVelocity = abs(roll);
        if(roll > 0)
            zAngle = -rotationVelocity;
        else if(roll < 0)
            zAngle = rotationVelocity;
    }

    if(cruiseMod && !triggerLock){
        cruiseFlight = !cruiseFlight;
        triggerLock = true;
    }else{
        triggerLock = false;
    }
        

    /*rotationVelocity = 50 * deltaTime;
    if (left)
        xAngle = rotationVelocity;
    if (right)
        xAngle = -rotationVelocity;*/

    rotationVelocity = 50 * deltaTime;
    if (down){
        yAngle = (pitch > -20) ? -rotationVelocity : 0;
        //cruiseFlight = false;
    }
    if (up){
        yAngle = (pitch < 20) ? rotationVelocity : 0;
        //cruiseFlight = false;
    }  
    if(!down && !up){
        if(pitch < 1 && pitch > -1)
            rotationVelocity = abs(pitch);
        if(pitch > 0)
            yAngle = -rotationVelocity;
        else if(pitch < 0)
            yAngle = rotationVelocity;
    }
    
    if(xAngle != 0 || zAngle != 0 || yAngle != 0){
        //std::cout << "Rotation\n";
        glm::quat quaternion = rotate(xAngle, yAngle, zAngle);
        Rigidbody::front = GameObject::front;
    }

    if(speedUp){
        thrustForce += speedRate * deltaTime;
    }
    if(slowDown){
        thrustForce -= slowRate * deltaTime;
        if(thrustForce < 0)
            thrustForce = 0;
    }

    if(cruiseFlight && pitch == 0)
        hVelocity = 0;

    glm::vec3 newForce(0.0, 0.0, 0.0);
    newForce += getLift() * glm::vec3(0.0f, GameObject::up.y, GameObject::up.z);
    newForce += getWeight() * glm::vec3(0.0f, -1.0f, 0.0f);
    newForce += getDrag() * -GameObject::front;
    newForce += thrustForce * GameObject::front;

    float verticalAcceleration = (sqrt(pow(newForce.x, 2) + pow(newForce.z, 2)) / m);
    float avgVe = vVelocity;
    if(GameObject::front.z * newForce.z > 0)
        avgVe += verticalAcceleration * deltaTime;
    else{
        avgVe -= verticalAcceleration * deltaTime;
        if(avgVe < 0.5f)
            avgVe = 0;
    }
    avgVe *= 0.5f;
    float sideForce = abs(getLift()) * sin(glm::radians(roll));
    if(sideForce != 0){
        float sideAcc = sideForce / 1.0f;
        float R = pow(avgVe, 2) / sideAcc;
        float angle = (avgVe / R) * deltaTime;
        glm::quat quaternion = bankingTurn(glm::degrees(-angle));
        Rigidbody::front = GameObject::front;
    }

    setForce(newForce);
    update(deltaTime);


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

glm::quat Uav::bankingTurn(float xAngle){
    float cosY = cos(glm::radians(xAngle/2));
    float sinY = sin(glm::radians(xAngle/2));

    //glm::vec3 newAxis =  glm::normalize(glm::vec3(0, up.y, up.z));
    glm::vec3 newAxis =  glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat qY = glm::quat(cosY, newAxis.x * sinY, newAxis.y * sinY, newAxis.z * sinY);

    orientation = glm::normalize(qY * orientation);

    GameObject::front = glm::normalize(qY * GameObject::front);
    up = glm::normalize(qY * up);
    right = glm::normalize(glm::cross(GameObject::front, up));

    return qY;
}

#endif