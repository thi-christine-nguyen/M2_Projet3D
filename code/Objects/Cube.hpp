#ifndef CUBE_HPP__
#define CUBE_HPP__

#include "code/GameObject.hpp"

class Cube : public GameObject {
public:
    float size;

    // Constructeur de Cube
    Cube(std::string name, float size, int textureID = 0, const char *texturePath = "",  GLuint programID = 0) {
        this->name = name;
        this->size = size;
        this->textureID = textureID;
        this->texturePath = texturePath;
        this->type = CUBE;

        // Init le plan + uvs inclus
        init();
        computeUV();
        //  Generer et charger les buffers
        this->GenerateBuffers(programID);
        // Init la bounding box
        this->initBoundingBox();
    }


    void init() {
        float half_size = size / 2.0f;

        // Vertices
        glm::vec3 v1(-half_size, -half_size, -half_size);
        glm::vec3 v2(half_size, -half_size, -half_size);
        glm::vec3 v3(half_size, half_size, -half_size);
        glm::vec3 v4(-half_size, half_size, -half_size);
        glm::vec3 v5(-half_size, -half_size, half_size);
        glm::vec3 v6(half_size, -half_size, half_size);
        glm::vec3 v7(half_size, half_size, half_size);
        glm::vec3 v8(-half_size, half_size, half_size);

        // Ajouter les vertices à la liste
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        vertices.push_back(v4);
        vertices.push_back(v5);
        vertices.push_back(v6);
        vertices.push_back(v7);
        vertices.push_back(v8);

        // Indices pour les faces
        indices = {
            0, 1, 2, 0, 2, 3, // face avant
            1, 5, 6, 1, 6, 2, // face droite
            4, 0, 3, 4, 3, 7, // face gauche
            5, 4, 7, 5, 7, 6, // face arriere
            3, 2, 6, 3, 6, 7, // face haut
            4, 5, 1, 4, 1, 0  // face bas
        };
    }

    void computeUV() {
        uvs.clear();

        // Face avant
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v1
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v2
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v3
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v4

        // Face droite
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v5
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v6
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v7
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v8

        // Face gauche
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v5
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v1
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v4
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v8

        // Face arrière
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v6
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v5
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v8
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v7

        // Face haut
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v4
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v3
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v7
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v8

        // Face bas
        uvs.push_back(glm::vec2(0.0f, 0.0f)); // v1
        uvs.push_back(glm::vec2(1.0f, 0.0f)); // v2
        uvs.push_back(glm::vec2(1.0f, 1.0f)); // v6
        uvs.push_back(glm::vec2(0.0f, 1.0f)); // v5

    }
};

#endif