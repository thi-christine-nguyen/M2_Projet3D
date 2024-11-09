#ifndef MESH_HPP__
#define MESH_HPP__
#include "GameObject.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>

class Mesh : public GameObject {
public:
    Mesh() {}

    Mesh(std::string name, const char *path, int textureID, const char *texturePath, GLuint programID = 0) {
        this->name = name;
        this->type = MESH;
        this->textureID = textureID;
        this->texturePath = texturePath;

        indices.clear();
        vertices.clear();
        uvs.clear();
        normals.clear();
        
        loadModel(path);
        
        this->GenerateBuffers(programID);
        this->initBoundingBox();
    }

private:
    void loadModel(const char *path) {
        std::string extension = std::string(path).substr(std::string(path).find_last_of(".") + 1);
  
        if (extension == "obj") {
            if (!loadOBJ(path)) {
                std::cerr << "Failed to load OBJ model." << std::endl;
            }
        } else if (extension == "off") {
            if (!loadOFF(path)) {
                std::cerr << "Failed to load OFF model." << std::endl;
            }
        } else {
            std::cerr << "Unsupported file format." << std::endl;
        }
    }

    bool loadOBJ(const char *path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::vector<glm::vec3> temp_vertices;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string type;
            ss >> type;
            

            if (type == "v") {
                glm::vec3 position;
                
                ss >> position.x >> position.y >> position.z;
                vertices.push_back(position);
            }
            else if (type == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
               
                normals.push_back(normal);
            }
            else if (type == "vt") {
                glm::vec2 texCoord;
                ss >> texCoord.x >> texCoord.y;
            
                uvs.push_back(texCoord);
            }
            else if (type == "f") {
                unsigned int posIdx[3], uvIdx[3], normIdx[3];
                char slash;
                for (int i = 0; i < 3; i++) {
                    ss >> posIdx[i] >> slash >> uvIdx[i] >> slash >> normIdx[i];
                    posIdx[i]--; uvIdx[i]--; normIdx[i]--;
                    indices.push_back(posIdx[i]);
                }
            }
        }

        

        file.close();
        return true;
    }

    bool loadOFF(const char *path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::string header;
        file >> header;
        if (header != "OFF") {
            std::cerr << "Not a valid OFF file." << std::endl;
            return false;
        }

        int vertexCount, faceCount, edgeCount;
        file >> vertexCount >> faceCount >> edgeCount;

        vertices.reserve(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            glm::vec3 position;
            file >> position.x >> position.y >> position.z;
            vertices.push_back(position);
        }

        for (int i = 0; i < faceCount; i++) {
            int numVertices;
            file >> numVertices;
            if (numVertices != 3) {
                std::cerr << "Only triangular faces are supported." << std::endl;
                return false;
            }
            unsigned int faceIdx[3];
            file >> faceIdx[0] >> faceIdx[1] >> faceIdx[2];
            indices.push_back(faceIdx[0]);
            indices.push_back(faceIdx[1]);
            indices.push_back(faceIdx[2]);
        }

        for (int i = 0; i < vertexCount; i++) {
            glm::vec3 position = vertices[i];
            // Exemple simple : on utilise la position en X et Y pour définir les coordonnées UV
            float u = (position.x + 1.0f) / 2.0f; // Normalisation pour l'UV
            float v = (position.y + 1.0f) / 2.0f;
            uvs.push_back(glm::vec2(u, v)); // Ajouter la coordonnée UV
        }

        file.close();
        return true;
    }

};

#endif