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
void processInput(GLFWwindow *window);
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

    glm::vec3 leftWing[8] = {
        glm::vec3(-1.0f, 0.02f, -0.2f),
        glm::vec3(-0.037f, 0.02f, -0.2f),
        glm::vec3(-1.0f, 0.02f, 0.0f),
        glm::vec3(-0.100f, 0.02f, 0.0f),

        glm::vec3(-1.0f, -0.02f, -0.2f),
        glm::vec3(-0.083f, -0.02f, -0.2f),
        glm::vec3(-1.0f, -0.02f, 0.0f),
        glm::vec3(-0.152f, -0.02f, 0.0f),
    };

    glm::vec3 rightWing[8] = {
        glm::vec3(0.037f, 0.02f, -0.2f),
        glm::vec3(1.0f, 0.02f, -0.2f),
        glm::vec3(0.100f, 0.02f, 0.0f),
        glm::vec3(1.0f, 0.02f, 0.0f),
        
        glm::vec3(0.083f, -0.02f, -0.2f),
        glm::vec3(1.0f, -0.02f, -0.2f),
        glm::vec3(0.152f, -0.02f, 0.0f),
        glm::vec3(1.0f, -0.02f, 0.0f),
    };

    glm::vec3 body[4] = {
        glm::vec3(0.0f, -0.03f, -0.5f),
        glm::vec3(0.2f, -0.03f, 0.1f),
        glm::vec3(-0.2f, -0.03f, 0.1f),
        glm::vec3(0.0f, 0.12f, 0.1f),
    };

    glm::vec3 leftTail[8] = {
        glm::vec3(-0.26f, 0.0f, 0.0f),
        glm::vec3(-0.24f, 0.0f, 0.0f),
        glm::vec3(-0.26f, 0.0f, 0.5f),
        glm::vec3(-0.24f, 0.0f, 0.5f),

        glm::vec3(-0.26f, -0.02f, 0.0f),
        glm::vec3(-0.24f, -0.02f, 0.0f),
        glm::vec3(-0.26f, -0.02f, 0.5f),
        glm::vec3(-0.24f, -0.02f, 0.5f),
    };

    glm::vec3 rightTail[8] = {
        glm::vec3(0.24f, 0.0f, 0.0f),
        glm::vec3(0.26f, 0.0f, 0.0f),
        glm::vec3(0.24f, 0.0f, 0.5f),
        glm::vec3(0.26f, 0.0f, 0.5f),

        glm::vec3(0.24f, -0.02f, 0.0f),
        glm::vec3(0.26f, -0.02f, 0.0f),
        glm::vec3(0.24f, -0.02f, 0.5f),
        glm::vec3(0.26f, -0.02f, 0.5f),
    };

    glm::vec3 tailBody[12] = {
        glm::vec3(-0.24f, 0.0f, 0.35f),
        glm::vec3(0.24f, 0.0f, 0.35f),
        glm::vec3(-0.24f, 0.0f, 0.5f),
        glm::vec3(0.24f, 0.0f, 0.5),

        glm::vec3(-0.24f, -0.02f, 0.35f),
        glm::vec3(0.24f, -0.02f, 0.35f),
        glm::vec3(-0.24f, -0.02f, 0.5f),
        glm::vec3(0.24f, -0.02f, 0.5),

        glm::vec3(0.0f, 0.2f, 0.35f),
        glm::vec3(0.0f, 0.2f, 0.5f),

        glm::vec3(0.0f, 0.18f, 0.35f),
        glm::vec3(0.0f, 0.18f, 0.5f),
    };

    float uavVertices[] = {
        // Top
        leftWing[0].x, leftWing[0].y, leftWing[0].z, 0.0f, 1.0f, 0.0f, 
        leftWing[1].x, leftWing[1].y, leftWing[1].z, 0.0f, 1.0f, 0.0f, 
        leftWing[2].x, leftWing[2].y, leftWing[2].z, 0.0f, 1.0f, 0.0f, 
        leftWing[3].x, leftWing[3].y, leftWing[3].z, 0.0f, 1.0f, 0.0f, 

        rightWing[0].x, rightWing[0].y, rightWing[0].z, 0.0f, 1.0f, 0.0f, 
        rightWing[1].x, rightWing[1].y, rightWing[1].z, 0.0f, 1.0f, 0.0f, 
        rightWing[2].x, rightWing[2].y, rightWing[2].z, 0.0f, 1.0f, 0.0f, 
        rightWing[3].x, rightWing[3].y, rightWing[3].z, 0.0f, 1.0f, 0.0f, 

        leftTail[0].x, leftTail[0].y, leftTail[0].z, 0.0f, 1.0f, 0.0f, 
        leftTail[1].x, leftTail[1].y, leftTail[1].z, 0.0f, 1.0f, 0.0f, 
        leftTail[2].x, leftTail[2].y, leftTail[2].z, 0.0f, 1.0f, 0.0f, 
        leftTail[3].x, leftTail[3].y, leftTail[3].z, 0.0f, 1.0f, 0.0f, 

        rightTail[0].x, rightTail[0].y, rightTail[0].z, 0.0f, 1.0f, 0.0f, 
        rightTail[1].x, rightTail[1].y, rightTail[1].z, 0.0f, 1.0f, 0.0f, 
        rightTail[2].x, rightTail[2].y, rightTail[2].z, 0.0f, 1.0f, 0.0f, 
        rightTail[3].x, rightTail[3].y, rightTail[3].z, 0.0f, 1.0f, 0.0f,

        // Bot
        leftWing[4].x, leftWing[4].y, leftWing[4].z, 0.0f, -1.0f, 0.0f, 
        leftWing[5].x, leftWing[5].y, leftWing[5].z, 0.0f, -1.0f, 0.0f, 
        leftWing[6].x, leftWing[6].y, leftWing[6].z, 0.0f, -1.0f, 0.0f, 
        leftWing[7].x, leftWing[7].y, leftWing[7].z, 0.0f, -1.0f, 0.0f, 

        rightWing[4].x, rightWing[4].y, rightWing[4].z, 0.0f, -1.0f, 0.0f, 
        rightWing[5].x, rightWing[5].y, rightWing[5].z, 0.0f, -1.0f, 0.0f, 
        rightWing[6].x, rightWing[6].y, rightWing[6].z, 0.0f, -1.0f, 0.0f, 
        rightWing[7].x, rightWing[7].y, rightWing[7].z, 0.0f, -1.0f, 0.0f, 

        leftTail[4].x, leftTail[4].y, leftTail[4].z, 0.0f, -1.0f, 0.0f, 
        leftTail[5].x, leftTail[5].y, leftTail[5].z, 0.0f, -1.0f, 0.0f, 
        leftTail[6].x, leftTail[6].y, leftTail[6].z, 0.0f, -1.0f, 0.0f, 
        leftTail[7].x, leftTail[7].y, leftTail[7].z, 0.0f, -1.0f, 0.0f, 

        rightTail[4].x, rightTail[4].y, rightTail[4].z, 0.0f, -1.0f, 0.0f, 
        rightTail[5].x, rightTail[5].y, rightTail[5].z, 0.0f, -1.0f, 0.0f, 
        rightTail[6].x, rightTail[6].y, rightTail[6].z, 0.0f, -1.0f, 0.0f, 
        rightTail[7].x, rightTail[7].y, rightTail[7].z, 0.0f, -1.0f, 0.0f, 

        body[0].x, body[0].y, body[0].z, 0.0f, -1.0f, 0.0f, 
        body[1].x, body[1].y, body[1].z, 0.0f, -1.0f, 0.0f, 
        body[2].x, body[2].y, body[2].z, 0.0f, -1.0f, 0.0f, 

        //Left
        leftWing[0].x, leftWing[0].y, leftWing[0].z, -1.0f, 0.0f, 0.0f,
        leftWing[2].x, leftWing[2].y, leftWing[2].z, -1.0f, 0.0f, 0.0f,
        leftWing[4].x, leftWing[4].y, leftWing[4].z, -1.0f, 0.0f, 0.0f,
        leftWing[6].x, leftWing[6].y, leftWing[6].z, -1.0f, 0.0f, 0.0f,

        leftTail[0].x, leftTail[0].y, leftTail[0].z, -1.0f, 0.0f, 0.0f,
        leftTail[2].x, leftTail[2].y, leftTail[2].z, -1.0f, 0.0f, 0.0f,
        leftTail[4].x, leftTail[4].y, leftTail[4].z, -1.0f, 0.0f, 0.0f,
        leftTail[6].x, leftTail[6].y, leftTail[6].z, -1.0f, 0.0f, 0.0f,

        rightTail[0].x, rightTail[0].y, rightTail[0].z, -1.0f, 0.0f, 0.0f,
        rightTail[2].x, rightTail[2].y, tailBody[1].z, -1.0f, 0.0f, 0.0f,
        rightTail[4].x, rightTail[4].y, rightTail[4].z, -1.0f, 0.0f, 0.0f,
        rightTail[6].x, rightTail[6].y, tailBody[5].z, -1.0f, 0.0f, 0.0f,

        //Right
        rightWing[1].x, rightWing[1].y, rightWing[1].z, 1.0f, 0.0f, 0.0f,
        rightWing[3].x, rightWing[3].y, rightWing[3].z, 1.0f, 0.0f, 0.0f,
        rightWing[5].x, rightWing[5].y, rightWing[5].z, 1.0f, 0.0f, 0.0f,
        rightWing[7].x, rightWing[7].y, rightWing[7].z, 1.0f, 0.0f, 0.0f,

        rightTail[1].x, rightTail[1].y, rightTail[1].z, 1.0f, 0.0f, 0.0f,
        rightTail[3].x, rightTail[3].y, rightTail[3].z, 1.0f, 0.0f, 0.0f,
        rightTail[5].x, rightTail[5].y, rightTail[5].z, 1.0f, 0.0f, 0.0f,
        rightTail[7].x, rightTail[7].y, rightTail[7].z, 1.0f, 0.0f, 0.0f,

        leftTail[1].x, leftTail[1].y, leftTail[1].z, 1.0f, 0.0f, 0.0f,
        leftTail[3].x, leftTail[3].y, tailBody[0].z, 1.0f, 0.0f, 0.0f,
        leftTail[5].x, leftTail[5].y, leftTail[5].z, 1.0f, 0.0f, 0.0f,
        leftTail[7].x, leftTail[7].y, tailBody[4].z, 1.0f, 0.0f, 0.0f,

        //FRONT
        leftWing[0].x, leftWing[0].y, leftWing[0].z, 0.0f, 0.0f, -1.0f,
        leftWing[1].x, leftWing[1].y, leftWing[1].z, 0.0f, 0.0f, -1.0f,
        leftWing[4].x, leftWing[4].y, leftWing[4].z, 0.0f, 0.0f, -1.0f,
        leftWing[5].x, leftWing[5].y, leftWing[5].z, 0.0f, 0.0f, -1.0f,

        rightWing[0].x, rightWing[0].y, rightWing[0].z, 0.0f, 0.0f, -1.0f,
        rightWing[1].x, rightWing[1].y, rightWing[1].z, 0.0f, 0.0f, -1.0f,
        rightWing[4].x, rightWing[4].y, rightWing[4].z, 0.0f, 0.0f, -1.0f,
        rightWing[5].x, rightWing[5].y, rightWing[5].z, 0.0f, 0.0f, -1.0f,

        tailBody[0].x, tailBody[0].y, tailBody[0].z, 0.0f, 0.0f, -1.0f,
        tailBody[8].x, tailBody[8].y, tailBody[8].z, 0.0f, 0.0f, -1.0f,
        tailBody[4].x, tailBody[4].y, tailBody[4].z, 0.0f, 0.0f, -1.0f,
        tailBody[10].x, tailBody[10].y, tailBody[10].z, 0.0f, 0.0f, -1.0f,

        tailBody[8].x, tailBody[8].y, tailBody[8].z, 0.0f, 0.0f, -1.0f,
        tailBody[1].x, tailBody[1].y, tailBody[1].z, 0.0f, 0.0f, -1.0f,
        tailBody[10].x, tailBody[10].y, tailBody[10].z, 0.0f, 0.0f, -1.0f,
        tailBody[5].x, tailBody[5].y, tailBody[5].z, 0.0f, 0.0f, -1.0f,

        //BACK
        leftWing[2].x, leftWing[2].y, leftWing[2].z, 0.0f, 0.0f, 1.0f,
        leftWing[3].x, leftWing[3].y, leftWing[3].z, 0.0f, 0.0f, 1.0f,
        leftWing[6].x, leftWing[6].y, leftWing[6].z, 0.0f, 0.0f, 1.0f,
        leftWing[7].x, leftWing[7].y, leftWing[7].z, 0.0f, 0.0f, 1.0f,

        rightWing[2].x, rightWing[2].y, rightWing[2].z, 0.0f, 0.0f, 1.0f,
        rightWing[3].x, rightWing[3].y, rightWing[3].z, 0.0f, 0.0f, 1.0f,
        rightWing[6].x, rightWing[6].y, rightWing[6].z, 0.0f, 0.0f, 1.0f,
        rightWing[7].x, rightWing[7].y, rightWing[7].z, 0.0f, 0.0f, 1.0f,

        body[1].x, body[1].y, body[1].z, 0.0f, 0.0f, 1.0f,
        body[2].x, body[2].y, body[2].z, 0.0f, 0.0f, 1.0f,
        body[3].x, body[3].y, body[3].z, 0.0f, 0.0f, 1.0f,

        leftTail[2].x, leftTail[2].y, leftTail[2].z, 0.0f, 0.0f, 1.0f,
        leftTail[3].x, leftTail[3].y, leftTail[3].z, 0.0f, 0.0f, 1.0f,
        leftTail[6].x, leftTail[6].y, leftTail[6].z, 0.0f, 0.0f, 1.0f,
        leftTail[7].x, leftTail[7].y, leftTail[7].z, 0.0f, 0.0f, 1.0f,

        rightTail[2].x, rightTail[2].y, rightTail[2].z, 0.0f, 0.0f, 1.0f,
        rightTail[3].x, rightTail[3].y, rightTail[3].z, 0.0f, 0.0f, 1.0f,
        rightTail[6].x, rightTail[6].y, rightTail[6].z, 0.0f, 0.0f, 1.0f,
        rightTail[7].x, rightTail[7].y, rightTail[7].z, 0.0f, 0.0f, 1.0f,

        tailBody[2].x, tailBody[2].y, tailBody[2].z, 0.0f, 0.0f, 1.0f,
        tailBody[9].x, tailBody[9].y, tailBody[9].z, 0.0f, 0.0f, 1.0f,
        tailBody[6].x, tailBody[6].y, tailBody[6].z, 0.0f, 0.0f, 1.0f,
        tailBody[11].x, tailBody[11].y, tailBody[11].z, 0.0f, 0.0f, 1.0f,

        tailBody[9].x, tailBody[9].y, tailBody[9].z, 0.0f, 0.0f, 1.0f,
        tailBody[3].x, tailBody[3].y, tailBody[3].z, 0.0f, 0.0f, 1.0f,
        tailBody[11].x, tailBody[11].y, tailBody[11].z, 0.0f, 0.0f, 1.0f,
        tailBody[7].x, tailBody[7].y, tailBody[7].z, 0.0f, 0.0f, 1.0f,

        // LEFT TOP
        body[0].x, body[0].y, body[0].z, -0.71f, 0.71f, 0.0f,
        body[2].x, body[2].y, body[2].z, -0.71f, 0.71f, 0.0f,
        body[3].x, body[3].y, body[3].z, -0.71f, 0.71f, 0.0f,

        tailBody[0].x, tailBody[0].y, tailBody[0].z, -0.71f, 0.71f, 0.0f,
        tailBody[8].x, tailBody[8].y, tailBody[8].z, -0.71f, 0.71f, 0.0f,
        tailBody[2].x, tailBody[2].y, tailBody[2].z, -0.71f, 0.71f, 0.0f,
        tailBody[9].x, tailBody[9].y, tailBody[9].z, -0.71f, 0.71f, 0.0f,

        // RIGH TOP
        body[0].x, body[0].y, body[0].z, 0.71f, 0.71f, 0.0f,
        body[1].x, body[1].y, body[1].z, 0.71f, 0.71f, 0.0f,
        body[3].x, body[3].y, body[3].z, 0.71f, 0.71f, 0.0f,

        tailBody[8].x, tailBody[8].y, tailBody[8].z, 0.71f, 0.71f, 0.0f,
        tailBody[1].x, tailBody[1].y, tailBody[1].z, 0.71f, 0.71f, 0.0f,
        tailBody[9].x, tailBody[9].y, tailBody[9].z, 0.71f, 0.71f, 0.0f,
        tailBody[3].x, tailBody[3].y, tailBody[3].z, 0.71f, 0.71f, 0.0f,
        
        // LEFT BOT
        tailBody[10].x, tailBody[10].y, tailBody[10].z, -0.71f, -0.71f, 0.0f,
        tailBody[5].x, tailBody[5].y, tailBody[5].z, -0.71f, -0.71f, 0.0f,
        tailBody[11].x, tailBody[11].y, tailBody[11].z, -0.71f, -0.71f, 0.0f,
        tailBody[7].x, tailBody[7].y, tailBody[7].z, -0.71f, -0.71f, 0.0f,

        // RIGHT BOT
        tailBody[4].x, tailBody[4].y, tailBody[4].z, 0.71f, -0.71f, 0.0f,
        tailBody[10].x, tailBody[10].y, tailBody[10].z, 0.71f, -0.71f, 0.0f,
        tailBody[6].x, tailBody[6].y, tailBody[6].z, 0.71f, -0.71f, 0.0f,
        tailBody[11].x, tailBody[11].y, tailBody[11].z, 0.71f, -0.71f, 0.0f,
    };

    unsigned int uavIndices[] = {
        // Top
        0, 1, 2,
        1, 2, 3,
        4, 5, 6,
        5, 6, 7,
        8, 9, 10,
        9, 10, 11,
        12, 13, 14,
        13, 14, 15,

        // Bot
        16, 17, 18,
        17, 18, 19,

        20, 21, 22,
        21, 22, 23,

        24, 25, 26,
        25, 26, 27,

        28, 29, 30,
        29, 30, 31,

        32, 33, 34,

        // LEFT
        35, 36, 37,
        36, 37, 38,

        39, 40, 41,
        40, 41, 42,

        43, 44, 45, 
        44, 45, 46,

        // RIGHT
        47, 48, 49,
        48, 49, 50,

        51, 52, 53,
        52, 53, 54,

        55, 56, 57,
        56, 57, 58,

        // FRONT 4 x 4
        59, 60, 61,
        60, 61, 62,

        63, 64, 65, 
        64, 65, 66, 

        67, 68, 69,
        68, 69, 70,

        71, 72, 73,
        72, 73, 74,

        // BACK 4 x 2 + 3 + 4 x 4
        75, 76, 77,
        76, 77, 78,

        79, 80, 81,
        80, 81, 82,

        83, 84, 85,

        86, 87, 88,
        87, 88, 89,

        90, 91, 92,
        91, 92, 93,

        94, 95, 96,
        95, 96, 97,

        98, 99, 100,
        99, 100, 101,

        // LEFT TOP 3 + 4
        102, 103, 104,

        105, 106, 107,
        106, 107, 108,

        // RIGH TOP 3 + 4
        109, 110, 111,

        112, 113, 114,
        113, 114, 115,

        // LEFT BOT 4
        116, 117, 118,
        117, 118, 119,

        // RIGHT BOT 4
        120, 121, 122,
        121, 122, 123
    };

    float targetVertices[] = {
        // FRONT
        // Left Top
        -1.0f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        // Left Bot
        -1.0f, -1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Left Top 2
        -0.8f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        // Left Bot 2
        -0.8f, -1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        // Right Top  
        0.8f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        // Right Bot 
        0.8f, -1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // Right Top 2
        1.0f, 1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        // Right Bot 2
        1.0f, -1.0f, 0.2f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        // BACK
        // Left Top
        -1.0f, 1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        // Left Bot
        -1.0f, -1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        // Left Top 2
        -0.8f, 1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        // Left Bot 2
        -0.8f, -1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        // Right Top 
        0.8f, 1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        // Right Bot 
        0.8f, -1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        // Right Top 2
        1.0f, 1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        // Right Bot 2
        1.0f, -1.0f, -0.2f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,

        // LEFT
        // Left Top
        -1.0f, 1.0f, 0.2f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // Left Bot
        -1.0f, -1.0f, 0.2f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Left Top
        -1.0f, 1.0f, -0.2f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        // Left Bot
        -1.0f, -1.0f, -0.2f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        // Right Top 
        0.8f, 1.0f, 0.2f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // Right Bot 
        0.8f, -1.0f, 0.2f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Right Top 
        0.8f, 1.0f, -0.2f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        // Right Bot 
        0.8f, -1.0f, -0.2f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        //RIGHT
        // Left Top 2
        -0.8f, 1.0f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // Left Bot 2
        -0.8f, -1.0f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Left Top 2
        -0.8f, 1.0f, -0.2f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        // Left Bot 2
        -0.8f, -1.0f, -0.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        // Right Top 2
        1.0f, 1.0f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        // Right Bot 2
        1.0f, -1.0f, 0.2f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // Right Top 2
        1.0f, 1.0f, -0.2f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        // Right Bot 2
        1.0f, -1.0f, -0.2f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        //TOP
        // Left Top
        -1.0f, 1.0f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // Left Top
        -1.0f, 1.0f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // Right Top 2
        1.0f, 1.0f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        // Right Top 2
        1.0f, 1.0f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 
        // Left Bot
        -1.0f, -1.0f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        // Left Bot
        -1.0f, -1.0f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        // Right Bot 2
        1.0f, -1.0f, 0.2f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        // Right Bot 2
        1.0f, -1.0f, -0.2f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 

        //BOT
        // Left Bot
        -1.0f, -1.0f, 0.2f, 0.0f, -1.0f, 0.0f, 0.0f,  1.0f,
        // Left Bot
        -1.0f, -1.0f, -0.2f, 0.0f, -1.0f, 0.0f, 1.0f,  1.0f,
        // Right Bot 2
        1.0f, -1.0f, 0.2f, 0.0f, -1.0f, 0.0f, 1.0f,  0.0f,
        // Right Bot 2
        1.0f, -1.0f, -0.2f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f,
        // Left Top
        -1.0f, 1.0f, 0.2f, 0.0f, -1.0f, 0.0f, 0.0f,  1.0f,
        // Left Top
        -1.0f, 1.0f, -0.2f, 0.0f, -1.0f, 0.0f, 1.0f,  1.0f,
        // Right Top 2
        1.0f, 1.0f, 0.2f, 0.0f, -1.0f, 0.0f, 1.0f,  0.0f,
        // Right Top 2
        1.0f, 1.0f, -0.2f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f
    };

    unsigned int targetIndices[] = {
        //FRONT
        0, 1, 2,
        1, 2, 3,

        4, 5, 6,
        5, 6, 7,

        // BACK
        8, 9, 10,
        9, 10, 11,

        12, 13, 14,
        13, 14, 15,

        // LEFT
        16, 17, 18,
        17, 18, 19,

        20, 21, 22,
        21, 22, 23,

        // RIGHT
        24, 25, 26,
        25, 26, 27,

        28, 29, 30,
        29, 30, 31,

        // TOP
        32, 33, 34,
        33, 34, 35,

        36, 37, 38,
        37, 38, 39,

        // BOT
        40, 41, 42,
        41, 42, 43,

        44, 45, 46,
        45, 46, 47
    };

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

    vector<Vertex> targetVertexVector;
    vector<unsigned int> targetIndiceVector;

    for(int i = 0; i < sizeof(targetVertices)/sizeof(float); i+=8){
        Vertex v;
        v.Position = glm::vec3(targetVertices[i], targetVertices[i+1], targetVertices[i+2]);
        v.Normal = glm::vec3(targetVertices[i+3], targetVertices[i+4], targetVertices[i+5]);
        v.TexCoords = glm::vec2(targetVertices[i+6], targetVertices[i+7]);
        targetVertexVector.push_back(v);
    }

    for(int i = 0; i < sizeof(targetIndices)/sizeof(unsigned int); i++){
        targetIndiceVector.push_back(targetIndices[i]);
    }

    std::vector<Vertex> uavVertexVector;
    std::vector<unsigned int> uavIndiceVector;

    for(int i = 0; i < sizeof(uavVertices)/sizeof(float); i+=6){
        Vertex v;
        v.Position = glm::vec3(uavVertices[i], uavVertices[i+1], uavVertices[i+2]);
        v.Normal = glm::vec3(uavVertices[i+3], uavVertices[i+4], uavVertices[i+5]);
        v.TexCoords = glm::vec2(0.0f, 0.0f);
        uavVertexVector.push_back(v);
    }

    for(int i = 0; i < sizeof(uavIndices)/sizeof(unsigned int); i++){
        uavIndiceVector.push_back(uavIndices[i]);
    }


    unsigned int diffuseMap = loadTexture("container.png");
    unsigned int specularMap = loadTexture("container_specular.png");

    Texture t0{diffuseMap, "diffuse"};
    Texture t1{specularMap, "specular"};

    vector<Texture> textureVector{t0, t1};

    vector<GameObject> gameObjects;
    Mesh mesh(verticeVector, indiceVector, textureVector);
    Mesh mesh2(uavVertexVector, uavIndiceVector, textureVector);

    Mesh mesh3(targetVertexVector, targetIndiceVector);

    Uav player(mesh2, glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),  40);
    game.addPlayer(player);

    game.addColliderTest(mesh, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), DYNAMIC, glm::vec3(1.0f, 1.0f, 1.0f));

    for(int i = 1; i < 10; i++){
        if(i == 1){
            game.addObject(GameObject(mesh, glm::vec3(6000.0f, 1.0f, 6000.0f), glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)), GROUND);
            continue;
        }
        else if(i == 2){
            game.addObject(GameObject(mesh, glm::vec3(20.0f, 10.0f, 1.0f), glm::vec3(0.0f, 4.0f, -4000.0f), glm::vec3(0.0f, 0.0f, 0.0f)), STATIC);
            continue;
        }
        float angle = 20.0f * i;
        game.addObject(GameObject(mesh, glm::vec3(1.0f, 1.0f, 1.0f), cubePositions[i], glm::vec3(angle, angle*0.3f, angle*0.5f)), STATIC);
    }

    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(0.0f, 80.0f, -200.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(50.0f, 80.0f, -400.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(100.0f, 80.0f, -600.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(150.0f, 80.0f, -800.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(200.0f, 80.0f, -1000.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(21.0f, 80.0f, -4000.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(21.0f, 80.0f, -4400.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(21.0f, 80.0f, -4800.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(21.0f, 80.0f, -4900.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
    game.addTarget(GameObject(mesh3, glm::vec3(40.0f, 40.0f, 5.0f), glm::vec3(21.0f, 80.0f, -6000.0f), glm::vec3(0.0f, 0.0f, 0.0f)));


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

void processInput(GLFWwindow *window)
{
    
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
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(ROLL_RIGHT, deltaTime);
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