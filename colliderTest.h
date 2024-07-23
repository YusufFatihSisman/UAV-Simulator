#ifndef COLLIDERTEST_H
#define COLLIDERTEST_H


#include "gameObject.h"
#include "collider.h"

class ColliderTest : public GameObject, public Collider{

    public:
        using GameObject::position;

        ColliderTest(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f)) 
        : GameObject(mesh, position, up, front), Collider(objectVertices, size, objectScale){}

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, float deltaTime);
};

void ColliderTest::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp,float deltaTime){
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
        rotate(xAngle, yAngle, zAngle);
        Collider::front = GameObject::front;
        Collider::right = GameObject::right;
        Collider::up = GameObject::up;
    }

    if(speedUp){
        GameObject::position += 5 * deltaTime * GameObject::front;
        Collider::position = GameObject::position;
    }

    update(orientation);
}

#endif