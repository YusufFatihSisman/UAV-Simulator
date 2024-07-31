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

class Game{
    public:

        Game(){
            camera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));
        }

        void start();
        void update(GLFWwindow* window);
        bool checkCollision(GameObject object, CollisionInfo &colInfo);
        void draw(bool cT = false);
        void drawColliders(bool cT = false);

        void processInput(GLFWwindow *window, bool cT = false);

        void addShader(ShaderType shaderType, const char* vertexPath, const char* fragmentPath);
        void addPlayer(const Uav &player, float* objectVertices, int size, ColliderType type = DYNAMIC);
        void addPlayer(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), ColliderType type = DYNAMIC);
        void addObject(const GameObject &object, float* objectVertices, int size, ColliderType type = STATIC);
        void addColliderTest(const Mesh &mesh, float* objectVertices, int size, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), ColliderType type = STATIC, glm::vec3 objectScale = glm::vec3(1.0f, 1.0f, 1.0f));

    private:
        Camera camera;
        Uav player;
        ColliderTest colTest;
        vector<GameObject> gameObjects;
        CustomShader shaders[SHADER_MAX];
        float deltaTime = 0.0f;
        float lastFrame = 0.0f;

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

}

void Game::update(GLFWwindow* window){
    bool cT = false;
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    fpsCounter++;

    if(currentFrame - lastFpsTime > 1){
        lastFpsTime = currentFrame;
        cout << "FPS: " << fpsCounter << "\n";
        player.printInfo();
        fpsCounter = 0;
    }

    processInput(window, cT);

    camera.Position.x = player.position.x;
    camera.Position.y = player.position.y + 2;
    camera.Position.z = player.position.z + 4;
    camera.Front = glm::normalize(player.position - camera.Position);

    /*camera.Position.x = colTest.position.x;
    camera.Position.y = colTest.position.y;
    camera.Position.z = colTest.position.z + 3;*/

    draw(cT);
    drawColliders(cT);
}

bool Game::checkCollision(GameObject object, CollisionInfo &colInfo){
    for(unsigned int i = 0; i < gameObjects.size(); i++){
        if(object.hit(gameObjects[i], colInfo))
            return true;  
    }
    return false;
}

void Game::draw(bool cT){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shaders[OBJECT].use();

    shaders[OBJECT].setVec3("viewPos", camera.Position);
    shaders[OBJECT].setFloat("material.shininess", 32.0f);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
    glm::mat4 view = camera.GetViewMatrix();

    shaders[OBJECT].setMat4("projection", projection);
    shaders[OBJECT].setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(!cT){
        //player.printInfo();
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
    
    
    
    for(unsigned int i = 0; i < gameObjects.size(); i++){
        model = glm::mat4(1.0f);
        model = glm::translate(model, gameObjects[i].position);
        model = model * gameObjects[i].getRotationMatrix();
        model = glm::scale(model, gameObjects[i].scale);
        shaders[OBJECT].setMat4("model", model);
        gameObjects[i].draw(shaders[OBJECT]);
    }
}

void Game::drawColliders(bool cT){
    glDisable(GL_DEPTH_TEST);
    shaders[COLLIDER].use();

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    shaders[COLLIDER].setMat4("projection", projection);
    shaders[COLLIDER].setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    
    if(!cT){
        model = glm::translate(model, player.position);
        model = model * player.getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);
        player.collider->draw(shaders[COLLIDER]);
    }else{
        model = glm::translate(model, colTest.position);
        model = model * colTest.getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);
        colTest.collider->draw(shaders[COLLIDER]);
    }
    
    for (unsigned int i = 0; i < gameObjects.size(); i++){
        model = glm::mat4(1.0f);
        model = glm::translate(model, gameObjects[i].position);
        model = model * gameObjects[i].getRotationMatrix();
        shaders[COLLIDER].setMat4("model", model);            
        gameObjects[i].collider->draw(shaders[COLLIDER]);
    }

    glEnable(GL_DEPTH_TEST);
}

void Game::processInput(GLFWwindow *window, bool cT){
    bool q = false, e = false, right = false, left = false, up = false, down = false, speedUp = false, slowDown = false, cruiseMod = false;
    
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

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
        right = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(YAW_LEFT, deltaTime);
        left = true;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(PITCH_UP, deltaTime);
        up = true;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(PITCH_DOWN, deltaTime);
        down = true;
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
        player.processInput(q, e, right, left, up, down, speedUp, slowDown, cruiseMod, deltaTime, gameObjects);
    else
        colTest.processInput(q, e, right, left, up, down, speedUp, deltaTime, gameObjects);
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

void Game::addObject(const GameObject &object, float* objectVertices, int size, ColliderType type){
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