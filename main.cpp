#include <iostream>
#include <cmath>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "game.h"
#include "customShader.h"
#include "camera.h"
//#include "mesh.h"
#include "gameObject.h"
#include "uav.h"
#include "collider.h"
#include "colliderTest.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, Uav &player, ColliderTest &colTest);
unsigned int loadTexture(const char *path);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool lockHold = false;

glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    
    Game game;
    game.addShader(OBJECT, "vertex.GLSL", "fragment.GLSL");
    game.addShader(LIGHT, "lightVertex.GLSL", "lightFragment.GLSL");
    game.addShader(COLLIDER, "lineVertex.GLSL", "lineFragment.GLSL");

    // left bot --- right bot --- top right --- top left
    float vertices[] = {
        // arka
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, // left bot back
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, // right bot back
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, // right up back
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, // left up back

        // ön
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f, // left bot front
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f, // right top front
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f, // r t f
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f, // l t f

        // sol
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // l t f
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f, // l t b
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // l b b
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f, // l b f

        // sağ
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f, // r t f
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f, // r t b
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f, // r b b
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f, // r b f

        // alt
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f, // l b b
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f, // r b b
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, // r b f
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f, // l b f

        // üst
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f, // l t b
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f, // r t b
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, // r t f
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f  // l t f
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3,

        4, 5, 7,
        5, 6, 7,

        8, 9, 11,
        9, 10, 11,

        12, 13, 15,
        13, 14, 15,

        16, 17, 19,
        17, 18, 19,

        20, 21, 23,
        21, 22, 23
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };

    /*
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };
    */
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  0.0f, -15.0f), 
        glm::vec3(-1.5f,  0.0f, -2.5f),  
        glm::vec3(-3.8f,  0.0f, -12.3f),  
        glm::vec3( 2.4f,  0.0f, -3.5f),  
        glm::vec3(-1.7f,  0.0f, -7.5f),  
        glm::vec3( 1.3f,  0.0f, -2.5f),  
        glm::vec3( 1.5f,  0.0f, -2.5f), 
        glm::vec3( 1.5f,  0.0f, -1.5f), 
        glm::vec3(-1.3f,  0.0f, -1.5f)  
    };
    
    // left bot --- right bot --- top right --- top left
    float objectVertices[] = {
        -0.5f, 0.0f, 0.5f,  -1.0f,  0.0f, 0.0f,  0.0f,  0.0f,  // left forward
         0.0f, 0.5f, 0.0f,  0.0f,  1.0f, 0.0f,  0.0f,  0.0f,   // mid top
         0.0f, 0.0f, -1.0f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,   // mid backward
         0.5f, 0.0f, 0.5f,  1.0f,  0.0f, 0.0f,  0.0f,  0.0f,  // right forward
    };

    vector<Vertex> objectVertexVector;
    vector<unsigned int> objectIndiceVector{0,1,2,3,1,2};
    for(int i = 0; i < sizeof(objectVertices)/sizeof(float); i+=8){
        Vertex v;
        v.Position = glm::vec3(objectVertices[i], objectVertices[i+1], objectVertices[i+2]);
        v.Normal = glm::vec3(objectVertices[i+3], objectVertices[i+4], objectVertices[i+5]);
        v.TexCoords = glm::vec2(objectVertices[i+6], objectVertices[i+7]);
        objectVertexVector.push_back(v);
    }

    vector<Vertex> verticeVector;
    vector<unsigned int> indiceVector;

    for(int i = 0; i < sizeof(vertices)/sizeof(float); i+=8){
        Vertex v;
        v.Position = glm::vec3(vertices[i], vertices[i+1], vertices[i+2]);
        v.Normal = glm::vec3(vertices[i+3], vertices[i+4], vertices[i+5]);
        v.TexCoords = glm::vec2(vertices[i+6], vertices[i+7]);
        verticeVector.push_back(v);
    }

    for(int i = 0; i < sizeof(indices)/sizeof(unsigned int); i++){
        indiceVector.push_back(indices[i]);
    }

    unsigned int diffuseMap = loadTexture("container.png");
    unsigned int specularMap = loadTexture("container_specular.png");

    Texture t0{diffuseMap, "diffuse"};
    Texture t1{specularMap, "specular"};

    vector<Texture> textureVector{t0, t1};

    vector<GameObject> gameObjects;
    Mesh mesh(verticeVector, indiceVector, textureVector);
    Mesh mesh2(objectVertexVector, objectIndiceVector, textureVector);

    Uav player(mesh2, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),  40);
    //game.addPlayer(mesh2, objectVertices, sizeof(objectVertices)/sizeof(float), glm::vec3(0.0f, 0.0f, 0.0f), DYNAMIC);
    game.addPlayer(player, objectVertices, sizeof(objectVertices)/sizeof(float));

    game.addColliderTest(mesh, vertices, sizeof(vertices)/sizeof(float), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), DYNAMIC, glm::vec3(1.0f, 1.0f, 1.0f));

    for(int i = 1; i < 10; i++){
        if(i == 1){
            game.addObject(GameObject(mesh, glm::vec3(200.0f, 1.0f, 4000.0f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),vertices, sizeof(vertices)/sizeof(float), GROUND);
            continue;
        }
        else if(i == 2){
            game.addObject(GameObject(mesh, glm::vec3(20.0f, 10.0f, 1.0f), glm::vec3(0.0f, 4.0f, -4000.0f), glm::vec3(0.0f, 0.0f, 0.0f)),vertices, sizeof(vertices)/sizeof(float), STATIC);
            continue;
        }
        float angle = 20.0f * i;
        game.addObject(GameObject(mesh, glm::vec3(1.0f, 1.0f, 1.0f), cubePositions[i], glm::vec3(angle, angle*0.3f, angle*0.5f)), vertices, sizeof(vertices)/sizeof(float), STATIC);
    }

    float timer = glfwGetTime();
    glEnable(GL_DEPTH_TEST);

    game.start();

    while(!glfwWindowShouldClose(window))
    {
        game.update(window);
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwTerminate();
    
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void processInput(GLFWwindow *window, Uav &player, ColliderTest &colTest)
{
    bool q = false, e = false, right = false, left = false, up = false, down = false, speedUp = false, slowDown = false, speedLock = false;
    
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
    if(!lockHold && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        speedLock = true;
        lockHold = true;
    }
    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE){
        lockHold = false;
    }

    //player.processInput(q, e, right, left, up, down, speedUp, slowDown, speedLock, deltaTime);
    //colTest.processInput(q, e, right, left, up, down, speedUp, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    /*if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }*/

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    //float yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}