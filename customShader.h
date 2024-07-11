#ifndef CUSTOMSHADER_H
#define CUSTOMSHADER_H

#include "glm/glm.hpp"

#include "shader.h"

class CustomShader : public Shader{

    public:
        CustomShader(const char* vertexPath, const char* fragmentPath) : Shader(vertexPath, fragmentPath){}

        void setPointLight(int index, const glm::vec3 &position, const glm::vec3 &ambient, const glm::vec3 &diffuse, const glm::vec3 &specular, float constant, float linear, float quadratic){
            std::string s = "pointLights[" + std::to_string(index) + "].";
            this->setVec3((s + "position").c_str(), position);
            this->setVec3((s + "ambient").c_str(), ambient);
            this->setVec3((s + "diffuse").c_str(), diffuse);
            this->setVec3((s + "specular").c_str(), specular);
            this->setFloat((s + "constant").c_str(), constant);
            this->setFloat((s + "linear").c_str(), linear);
            this->setFloat((s + "quadratic").c_str(), quadratic);
        }
};

#endif