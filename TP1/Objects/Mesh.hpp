#ifndef MESH_HPP__
#define MESH_HPP__
#include "TP1/GameObject.hpp"
#include "common/objloader.hpp"

class Mesh : public GameObject {
public:
    
    // Constructeur de Plane
    Mesh() {}


    Mesh(std::string name, const char *path, int textureID, const char *texturePath, GLuint programID = 0) {
        this->name = name;
        this->type = MESH;
        this->textureID = textureID;
        this->texturePath = texturePath;

        indices.clear(); 
        vertices.clear(); 
        uvs.clear(); 
        std::vector<glm::vec3> out_normals; 

        loadOBJ(path, vertices, uvs, out_normals, indices); 
        this->GenerateBuffers(programID);
        this->initBoundingBox();

    }
};

#endif