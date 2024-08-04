#ifndef GAME_H
#define GAME_H

#include<iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include "camera.h"
#include "gameObject.h"
#include "uav.h"
#include "customShader.h"
#include "colliderTest.h"

using namespace std;

enum ShaderType{
    OBJECT,
    LIGHT,
    COLLIDER,
    SHADER_MAX
};

struct ColState{
    CollisionInfo info;
    glm::vec3 position;
    unsigned int index;
};


class Game{
    public:
        Game(){
            camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
        }

        ~Game(){
            delete colInfos;

            for(int i = 0; i < SHADER_MAX; i++){
                shaders[i].terminate();
            }
            for(int i = 0; i < gameObjects.size(); i++){
                if(!gameObjects[i].isDestroyed)
                    gameObjects[i].destroy();
            }

            player.destroy();
            colTest.destroy();

        }

        void start();
        void update(GLFWwindow* window);
        
        void addShader(ShaderType shaderType, const char* vertexPath, const char* fragmentPath);
        void addPlayer(const Uav &player, float* objectVertices, int size, ColliderType type = DYNAMIC);
        void addPlayer(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), ColliderType type = DYNAMIC);
        void addObject(const GameObject &object, float* objectVertices, int size, ColliderType type = STATIC);
        void addTarget(const GameObject &object, float* objectVertices, int size);
        void addColliderTest(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), ColliderType type = STATIC, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f));

    private:
        void draw();
        void drawColliders();

        void processInput(GLFWwindow *window);
        void checkCollisions();

        bool cT = false;

        Camera camera;
        Uav player;
        ColliderTest colTest;
        vector<GameObject> gameObjects;
        CustomShader shaders[SHADER_MAX];
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        ColState* colInfos;

        bool destroyRequest = false;
        unsigned int firstTargetIndex = -1;

        const unsigned int SCR_WIDTH = 800;
        const unsigned int SCR_HEIGHT = 600;

        glm::vec3 pointLightPositions[4] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
        };

        unsigned int fpsCounter = 0;
        float lastFpsTime = 0.0f;
};


void Game::start(){
    shaders[OBJECT].use();
    shaders[OBJECT].setDirectionLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.5f, 0.5f, 0.5f));
    shaders[OBJECT].setPointLight(0, pointLightPositions[0], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
    shaders[OBJECT].setPointLight(1, pointLightPositions[1], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
    shaders[OBJECT].setPointLight(2, pointLightPositions[2], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);
    shaders[OBJECT].setPointLight(3, pointLightPositions[3], glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f);

    colInfos = new ColState[gameObjects.size()];
}

void Game::update(GLFWwindow* window){
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    fpsCounter++;

    if(currentFrame - lastFpsTime > 1){
        lastFpsTime = currentFrame;
        cout << "FPS: " << fpsCounter << "\n";
        fpsCounter = 0;
        if(!cT)
            player.printInfo();
    }

    processInput(window);
    checkCollisions();

    if(!cT){
        camera.Front.x = player.GameObject::front.x;
        camera.Front.z = player.GameObject::front.z;
        camera.Front = glm::normalize(camera.Front);
        camera.Position = player.position - 6.0f * camera.Front;
    }else{
        camera.Position = colTest.position - 6.0f * colTest.front;
        camera.Front = colTest.front;
    }

    draw();
    drawColliders();
}

void Game::draw(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders[OBJECT].use();

    shaders[OBJECT].setVec3("viewPos", camera.Position);
    shaders[OBJECT].setFloat("material.shininess", 32.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
    glm::mat4 view = camera.GetViewMatrix();

    shaders[OBJECT].setMat4("projection", projection);
    shaders[OBJECT].setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(!cT){
        model = glm::translate(model, player.position);
        model = model * player.getRotationMatrix();
        model = glm::scale(model, player.scale);
        shaders[OBJECT].setMat4("model", model);
        player.draw(shaders[OBJECT]);
    }else{
        model = glm::translate(model, colTest.position);
        model = model * colTest.getRotationMatrix();
        shaders[OBJECT].setMat4("model", model);
        colTest.draw(shaders[OBJECT]);
    }

    int objectSize = firstTargetIndex == -1 ? gameObjects.size() : firstTargetIndex;
    for(unsigned int i = 0; i < objectSize; i++){
        if(gameObjects[i].isDestroyed)
            continue;
        model = glm::mat4(1.0f);
        model = glm::translate(model, gameObjects[i].position);
        model = model * gameObjects[i].getRotationMatrix();
        model = glm::scale(model, gameObjects[i].scale);
        shaders[OBJECT].setMat4("model", model);
        gameObjects[i].draw(shaders[OBJECT]);
        
    }

    shaders[LIGHT].use();
    shaders[LIGHT].setVec3("viewPos", camera.Position);
    shaders[LIGHT].setMat4("projection", projection);
    shaders[LIGHT].setMat4("view", view);
    for(unsigned int i = objectSize; i < gameObjects.size(); i++){
        if(gameObjects[i].isDestroyed)
            continue;
        model = glm::mat4(1.0f);
        model = glm::translate(model, gameObjects[i].position);
        model = model * gameObjects[i].getRotationMatrix();
        model = glm::scale(model, gameObjects[i].scale * glm::vec3(1.1f, 1.1f, 2.0f));
        shaders[LIGHT].setMat4("model", model);
        gameObjects[i].draw(shaders[LIGHT]);
    }
    /*
    shaders[LIGHT].use();
    shaders[LIGHT].setVec3("viewPos", camera.Position);
    shaders[LIGHT].setMat4("projection", projection);
    shaders[LIGHT].setMat4("view", view);

    for(unsigned int i = 0; i < targets.size(); i++){
        if(targets[i].isDestroyed)
            continue;
        model = glm::mat4(1.0f);
        model = glm::translate(model, targets[i].position);
        model = model * targets[i].getRotationMatrix();
        model = glm::scale(model, targets[i].scale * glm::vec3(1.2f, 1.2f, 2.0f));
        shaders[LIGHT].setMat4("model", model);
        targets[i].draw(shaders[LIGHT]);
    }
    */
}

void Game::drawColliders(){
    glDisable(GL_DEPTH_TEST);
    shaders[COLLIDER].use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shaders[COLLIDER].setMat4("projection", projection);
    shaders[COLLIDER].setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(!cT){
        model = glm::translate(model, player.collider->position);
        model = model * player.getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);
        player.collider->draw(shaders[COLLIDER]);
    }else{
        model = glm::translate(model, colTest.collider->position);
        model = model * colTest.getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);
        colTest.collider->draw(shaders[COLLIDER]);
    }
    
    for (unsigned int i = 0; i < gameObjects.size(); i++){
        if(gameObjects[i].isDestroyed)
            continue;
        model = glm::mat4(1.0f);
        model = glm::translate(model, gameObjects[i].collider->position);
        model = model * gameObjects[i].getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);            
        gameObjects[i].collider->draw(shaders[COLLIDER]);
    }
    /*
    for(unsigned int i = 0; i < targets.size(); i++){
        if(targets[i].isDestroyed)
            continue;
        model = glm::mat4(1.0f);
        model = glm::translate(model, targets[i].position);
        model = model * targets[i].getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);
        targets[i].collider->draw(shaders[LIGHT]);
    }
    */

    glEnable(GL_DEPTH_TEST);
}

void Game::processInput(GLFWwindow *window){
    bool q = false, e = false, right = false, left = false, up = false, down = false, speedUp = false, slowDown = false, cruiseMod = false;
    
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camera.ProcessKeyboard(FORWARD, deltaTime);
        up = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        down = true;
    }
        
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camera.ProcessKeyboard(LEFT, deltaTime);
        left = true;
    }
        
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camera.ProcessKeyboard(RIGHT, deltaTime);
        right = true;
    }
        

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(ROLL_LEFT, deltaTime);
        q = true;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(ROLL_RIGHT, deltaTime);
        e = true;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(YAW_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(YAW_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(PITCH_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(PITCH_DOWN, deltaTime);
    }

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        speedUp = true;
    }
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        slowDown = true;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        cruiseMod = true;
    }

    if(!cT)
        player.processInput(right, left, up, down, speedUp, slowDown, cruiseMod, deltaTime);
    else
        colTest.processInput(q, e, right, left, up, down, speedUp, deltaTime);
}

void Game::checkCollisions(){
    CollisionInfo colInfo;
    int colSize = 0;
    if(cT){
        if(colTest.collider != NULL && colTest.isMoved){
            glm::vec3 finalPos = colTest.collider->position;
            for(unsigned int i = 0; i < gameObjects.size(); i++){
                if(gameObjects[i].isDestroyed)
                    continue;
                colTest.collider->set(finalPos, colTest.GameObject::front, colTest.GameObject::up, colTest.GameObject::right);
                colTest.collider->update(colTest.orientation);
                if(colTest.hit(gameObjects[i], colInfo)){
                    ColState currentColState{colInfo, colTest.collider->position, i};
                    for(unsigned int j = 0; j < colSize; j++){
                        if(glm::length(colTest.position - currentColState.info.pointOnPlane) < glm::length(colTest.position - colInfos[j].info.pointOnPlane)){
                            ColState tempCol = colInfos[j];

                            colInfos[j] = currentColState;
                            currentColState = tempCol;
                        }
                    }
                    colInfos[colSize] = currentColState;
                    colSize++;
                }       
            }
            for(unsigned int i = 0; i < colSize; i++){
                colTest.collider->position = colInfos[i].position;
                colTest.collider->onHit(colInfos[i].info);
                if(colInfos[i].info.type == STATIC){
                    finalPos = colTest.collider->position;
                }
                else if(colInfos[i].info.type == GROUND){
                    finalPos = colTest.collider->position;
                }
                else if(colInfos[i].info.type == SPEACIAL){
                    std::cout << "SPEACIAL HIT\n";
                    destroyRequest = true;
                    gameObjects[colInfos[i].index].isDestroyed = true;
                    gameObjects[colInfos[i].index].destroy();
                }
            }
            colTest.GameObject::position = finalPos;
            colTest.collider->position = colTest.GameObject::position;
            colTest.collider->update(colTest.orientation);
            colTest.isMoved = false;
        }
        return;
    }
    if(player.collider != NULL && player.isMoved){
        for(unsigned int i = 0; i < gameObjects.size(); i++){
            if(gameObjects[i].isDestroyed)
                continue;
            player.collider->set(player.Rigidbody::position, player.GameObject::front, player.GameObject::up, player.GameObject::right);
            player.collider->update(player.orientation);

            if(player.hit(gameObjects[i], colInfo)){
                ColState currentColState{colInfo, player.collider->position, i};
                for(unsigned int j = 0; j < colSize; j++){
                    if(glm::length(player.position - currentColState.info.pointOnPlane) < glm::length(player.position - colInfos[j].info.pointOnPlane)){
                        ColState tempCol = colInfos[j];

                        colInfos[j] = currentColState;
                        currentColState = tempCol;
                    }
                }
                colInfos[colSize] = currentColState;
                colSize++;
            }
        }
        for(unsigned int i = 0; i < colSize; i++){
            player.onHit(colInfos[i].info);
            player.collider->position = colInfos[i].position;
            player.collider->onHit(colInfos[i].info);
            if(colInfos[i].info.type == STATIC){
                player.Rigidbody::position = player.collider->position;
            }
            else if(colInfos[i].info.type == GROUND){
                player.Rigidbody::position = player.collider->position;
            }
            else if(colInfos[i].info.type == SPEACIAL){
                destroyRequest = true;
                gameObjects[colInfos[i].index].isDestroyed = true;
                gameObjects[colInfos[i].index].destroy();
            }
        }
        player.GameObject::position = player.Rigidbody::position;
        player.collider->position = player.GameObject::position;
        player.collider->update(player.orientation);
        player.isMoved = false;
    }
}

void Game::addColliderTest(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position, glm::vec3 rotation, ColliderType type, glm::vec3 objectScale){
    colTest = ColliderTest(mesh, objectVertices, size, position, rotation, type, objectScale);
}

void Game::addPlayer(const Uav &player, float* objectVertices, int size, ColliderType type){
    this->player = player;
    this->player.addCollider(objectVertices, size, type);
    this->player.collider->update(player.orientation);
}

void Game::addPlayer(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position, ColliderType type){
    player = Uav(mesh, position);
    player.addCollider(objectVertices, size, type);
    this->player.collider->update(player.orientation);
}

void Game::addTarget(const GameObject &object, float* objectVertices, int size){
    if(firstTargetIndex == -1)
        firstTargetIndex = gameObjects.size();
    gameObjects.push_back(object);
    gameObjects[gameObjects.size()-1].addCollider(objectVertices, size, SPEACIAL);
    gameObjects[gameObjects.size()-1].collider->update(gameObjects[gameObjects.size()-1].orientation);
    /*targets.push_back(object);
    targets[targets.size()-1].addCollider(objectVertices, size, SPEACIAL);
    targets[targets.size()-1].collider->update(targets[targets.size()-1].orientation);*/
}

void Game::addObject(const GameObject &object, float* objectVertices, int size, ColliderType type){
    if(firstTargetIndex != -1)
        return;
    gameObjects.push_back(object);
    gameObjects[gameObjects.size()-1].addCollider(objectVertices, size, type);
    gameObjects[gameObjects.size()-1].collider->update(gameObjects[gameObjects.size()-1].orientation);
}

void Game::addShader(ShaderType shaderType, const char* vertexPath, const char* fragmentPath){
    if(shaderType >= SHADER_MAX || shaderType < 0)
        return;
    shaders[shaderType] = CustomShader(vertexPath, fragmentPath);
}

#endif