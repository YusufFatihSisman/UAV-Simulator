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
#include "gameObject.h"
#include "uav.h"
#include "collider.h"
#include "colliderTest.h"

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1200, 900, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    
    Game game;
    game.addShader(OBJECT, "shaders/vertex.GLSL", "shaders/fragment.GLSL");
    game.addShader(LIGHT, "shaders/lightVertex.GLSL", "shaders/lightFragment.GLSL");
    game.addShader(COLLIDER, "shaders/lineVertex.GLSL", "shaders/lineFragment.GLSL");

    game.addPlayer("models/uav", glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f),  40);

    game.addColliderTest("models/ground", glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), DYNAMIC, glm::vec3(1.0f, 1.0f, 1.0f));

    game.addObject("models/ground", glm::vec3(0.0f, -2.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(12000.0f, 1.0f, 12000.0f), GROUND);

    
    game.addTarget("models/target", glm::vec3(200.0f, 80.0f, -1400.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(100.0f, 80.0f, -1200.0f), glm::vec3(-30.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(0.0f, 80.0f, -1000.0f), glm::vec3(-60.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-400.0f, 80.0f, -600.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));


    game.addTarget("models/target", glm::vec3(-900.0f, 100.0f, -600.0f), glm::vec3(-90.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-1500.0f, 140.0f, -600.0f), glm::vec3(-90.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-2100.0f, 180.0f, -600.0f), glm::vec3(-90.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-2700.0f, 220.0f, -600.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));

    game.addTarget("models/target", glm::vec3(-3000.0f, 220.0f, -600.0f), glm::vec3(-120.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3400.0f, 220.0f, -900.0f), glm::vec3(-120.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3700.0f, 220.0f, -1300.0f), glm::vec3(-160.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3900.0f, 220.0f, -1900.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3900.0f, 220.0f, -2100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));

    game.addTarget("models/target", glm::vec3(-3900.0f, 180.0f, -2700.0f), glm::vec3(0.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3900.0f, 140.0f, -3300.0f), glm::vec3(0.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3900.0f, 100.0f, -3900.0f), glm::vec3(0.0f, -30.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3900.0f, 60.0f, -4700.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));


    game.addTarget("models/target", glm::vec3(-3600.0f, 60.0f, -5200.0f), glm::vec3(-30.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-3100.0f, 60.0f, -5700.0f), glm::vec3(-60.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-2500.0f, 60.0f, -6000.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));
    game.addTarget("models/target", glm::vec3(-2100.0f, 60.0f, -6000.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(40.0f, 40.0f, 5.0f));



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
