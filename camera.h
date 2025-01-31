#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    ROLL_LEFT,
    ROLL_RIGHT,
    YAW_LEFT,
    YAW_RIGHT,
    PITCH_UP,
    PITCH_DOWN,
};

// Default camera values
//const float YAW         = -90.0f;
const float YAW         = 0.0f;
const float PITCH       =  0.0f;
const float ROLL        = 0.0f;
//const float SPEED       =  2.5f;
const float SPEED       =  2.0f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;
    // camera options
    float MovementSpeed = 2.0f;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float roll = ROLL) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Up = up;
        Yaw = yaw;
        Pitch = pitch;
        Roll = roll;
        Right = glm::normalize(glm::cross(Front, WorldUp));
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float roll) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Up = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        Roll = roll;
        Right = glm::normalize(glm::cross(Front, WorldUp));
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;

        float rollVelocity = 50 * deltaTime;

        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;

        if (direction == ROLL_LEFT)
            Roll -= rollVelocity;
        if (direction == ROLL_RIGHT)
            Roll += rollVelocity;

        if (direction == YAW_LEFT)
            Yaw += rollVelocity;
        if (direction == YAW_RIGHT)
            Yaw -= rollVelocity;

        if (direction == PITCH_DOWN)
            Pitch -= rollVelocity;
        if (direction == PITCH_UP)
            Pitch += rollVelocity;

        //updateCameraVectors();
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = false)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }


    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {

        //R = [cos(a/2), sin(a/2)*x, sin(a/2)*y, sin(a/2)*z]
        float cosX = cos(glm::radians(Pitch/2));
        float sinX = sin(glm::radians(Pitch/2));

        float cosY = cos(glm::radians(Yaw/2));
        float sinY = sin(glm::radians(Yaw/2));
        
        float cosZ = cos(glm::radians(Roll/2));
        float sinZ = sin(glm::radians(Roll/2));

        glm::quat qX = glm::quat(cosX, Right.x * sinX, Right.y * sinX, Right.z * sinX);
        glm::quat qY = glm::quat(cosY, Up.x * sinY, Up.y * sinY, Up.z * sinY);
        glm::quat qZ = glm::quat(cosZ, Front.x * sinZ, Front.y * sinZ, Front.z * sinZ);


        glm::quat qCombined = qZ * qY * qX;
        qCombined = glm::normalize(qCombined);

        Front = glm::normalize(qCombined * Front);
        Right = glm::normalize(qCombined * Right);
        Up    = glm::normalize(glm::cross(Right, Front)); 

        Yaw = 0;
        Pitch = 0;
        Roll = 0;

        /*
        std::cout << "DIRECTIONS\n" << "Front: ";
        std::cout << Front.x << " " << Front.y << " " << Front.z << "\n";
        std::cout << "Up: ";
        std::cout << Up.x << " " << Up.y << " " << Up.z << "\n"; 
         std::cout << "Right: ";
        std::cout << Right.x << " " << Right.y << " " << Right.z << "\n"; 
        std::cout << "----------------\n";
        */
    }
};
#endif