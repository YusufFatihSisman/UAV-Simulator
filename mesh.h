#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream> 
#include <vector>
#include <string> 
#include <filesystem>

#include "glm/glm.hpp"
#include "stb_image.h"

#include "shader.h"
#include "commonStructs.h"

using namespace std;

struct Texture {
    unsigned int id;
    string type;
};  

unsigned int loadTexture(const char *path);

class Mesh{
    public:
        vector<Vertex>       vertices;
        vector<unsigned int> indices;
        vector<Texture>      textures;

        Mesh(){}

        Mesh(char const *path){
            loadData(path);
            setupMesh();
        }

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices){
            this->vertices = vertices;
            this->indices = indices;
            setupMesh();
        }

        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures){
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            setupMesh();
        }

        Mesh(const Mesh& mesh){
            this->vertices = mesh.vertices;
            this->indices = mesh.indices;
            this->textures = mesh.textures;
            this->VAO = mesh.VAO;
            this->VBO = mesh.VBO;
            this->EBO = mesh.EBO;
        }
    
        Mesh& operator=(const Mesh& mesh){
            this->vertices = mesh.vertices;
            this->indices = mesh.indices;
            this->textures = mesh.textures;
            this->VAO = mesh.VAO;
            this->VBO = mesh.VBO;
            this->EBO = mesh.EBO;
            return *this;
        }

        void free(){
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }

        void draw(Shader &shader);
    private:
        //  render data
        unsigned int VAO, VBO, EBO;

        void setupMesh();
        void loadData(char const *path);

};

void Mesh::draw(Shader &shader){
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
        // retrieve texture number (the N in diffuse_textureN)
        string number;
        string name = textures[i].type;
        if(name == "diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "specular")
            number = std::to_string(specularNr++);

        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    //glActiveTexture(GL_TEXTURE0);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::loadData(char const *path){
    string modelPath = path;
    string data = (modelPath + "/data.txt").c_str();
    string index = (modelPath + "/index.txt").c_str();
    string diffusePNG = (modelPath + "/diffuse.png").c_str();
    string diffuseJPG = (modelPath + "/diffuse.jpg").c_str();
    string specular = (modelPath + "/specular.png").c_str();

    ifstream dataFile(data); 
    
    string line; 
    string val;

    /*if(!filesystem::exists(data) || !filesystem::exists(index))
        return;*/
        
    while (getline(dataFile, line)) { 
        stringstream ls(line);
        int index = 0;
        float arr[8] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        while(getline(ls, val, ' ')){
            arr[index++] = std::stof(val);
        }   
        Vertex v = {glm::vec3(arr[0], arr[1], arr[2]), glm::vec3(arr[3], arr[4], arr[5]), glm::vec2(arr[6], arr[7])};
        vertices.push_back(v);
    } 
    dataFile.close();

    ifstream indexFile(index);
    while (getline(indexFile, val, ' ')) { 
        indices.push_back(stoul(val));
    } 
    indexFile.close();

    unsigned int diffuseMap = -1;
    unsigned int specularMap = -1;

    if(filesystem::exists(diffusePNG))
        diffuseMap = loadTexture(diffusePNG.c_str());
    else{
        if(filesystem::exists(diffuseJPG))
            diffuseMap = loadTexture(diffuseJPG.c_str());
    }
        
    if(filesystem::exists(specular))
        specularMap = loadTexture(specular.c_str());
    

    if(diffuseMap != -1)
        textures.push_back(Texture{diffuseMap, "diffuse"});
    if(specularMap != -1)
        textures.push_back(Texture{specularMap, "specular"});
}

void Mesh::setupMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), 
                 &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    if(textures.size() != 0){
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    }
    glBindVertexArray(0);
}

unsigned int loadTexture(char const * path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data){
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

#endif