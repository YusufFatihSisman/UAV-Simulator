#ifndef COLLIDERTEST_H
#define COLLIDERTEST_H

#include "game.h"
#include "gameObject.h"
#include "collider.h"

class ColliderTest : public GameObject{

    public:

        ColliderTest(){}

        ColliderTest(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
        glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), ColliderType type = STATIC, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f)) 
        : GameObject(mesh, objectScale, position, rotation){
            addCollider(objectVertices, size, DYNAMIC);
        }

        ColliderTest(const ColliderTest& cT) : GameObject(cT){}
    
        ColliderTest& operator=(const ColliderTest& cT){
            GameObject::operator = (cT);
            return *this;
        }

        void processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, float deltaTime, vector<GameObject> &gameObjects);

        void onHit(CollisionInfo col);
};

void ColliderTest::processInput(bool q, bool e, bool right, bool left, bool up, bool down, bool speedUp, float deltaTime, vector<GameObject> &gameObjects){
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
    
    if(q || e || right || left || up || down)
        rotate(xAngle, yAngle, zAngle);
    
    if(speedUp)
        GameObject::position += 5 * deltaTime * GameObject::front;

    if(collider != NULL){
        collider->set(position, GameObject::front, GameObject::up, GameObject::right);
        collider->update(orientation);
    }

    CollisionInfo colInfo;
    for(unsigned int i = 0; i < gameObjects.size(); i++){
        if(hit(gameObjects[i], colInfo))
            onHit(colInfo);
    }
}

void ColliderTest::onHit(CollisionInfo col){
    
}

#endif