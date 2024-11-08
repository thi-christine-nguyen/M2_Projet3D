#ifndef SPHERE_HPP__
#define SPHERE_HPP__

#include "TP1/GameObject.hpp"

class Sphere : public GameObject {
public:
    int resolution;
    float radius;

    // CONSTRUCTOR
    Sphere(std::string name, unsigned int res, unsigned int rad, int textureID = 0, const char *texturePath = "", GLuint programID = 0){
        this->name = name;
        this->textureID = textureID;
        this->texturePath = texturePath;
        this->resolution = res;
        this->radius = rad;
        this->type = SPHERE;

        // Init
        init();
        computeUV();
        //  Generer et charger les buffers
        this->GenerateBuffers(programID);
        // Init la bounding box
        this->initBoundingBox();
    }

    Sphere(std::string name, bool isPlayer, unsigned int res, unsigned int rad, int textureID = 0, const char *texturePath = "", GLuint programID = 0) {
        this->name = name;
        this->textureID = textureID;
        this->texturePath = texturePath;
        this->resolution = res;
        this->radius = rad;
        isPlayer ? this->type = PLAYER : this->type = SPHERE; 

        // Init
        init();
        computeUV();
        //  Generer et charger les buffers
        this->GenerateBuffers(programID);
        // Init la bounding box
        this->initBoundingBox();
    }



    void init()
    {
        indices.clear();
        vertices.clear();

        for (int i = 0; i <= resolution; ++i) {
            float theta = M_PI * (i / (float)resolution);
            float sin_theta = sin(theta);
            float cos_theta = cos(theta);

            for (int j = 0; j <= resolution; ++j) {
                float phi = 2 * M_PI * (j / (float)resolution);
                float sin_phi = sin(phi);
                float cos_phi = cos(phi);

                glm::vec3 vertex = glm::vec3(cos_phi * sin_theta, cos_theta, sin_phi * sin_theta);
                vertices.push_back(vertex * radius);
                // std::cout << vertex.x << " " << vertex.y << " " << vertex.z<< std::endl; 
            }
        }

        for (int i = 0; i < resolution; ++i) {
            for (int j = 0; j < resolution; ++j) {
                int i1 = i * (resolution + 1) + j;
                int i2 = i1 + 1;
                int i3 = (i + 1) * (resolution + 1) + j;
                int i4 = i3 + 1;

                indices.push_back(i1);
                indices.push_back(i3);
                indices.push_back(i2);

                indices.push_back(i2);
                indices.push_back(i3);
                indices.push_back(i4);
            }
        }

        
    };

    void computeUV()
    {
        uvs.clear();
        float uv_step = 1.0f / static_cast<float>(resolution);
        for (int i = 0; i < resolution + 1; i++)
        {
            for (int j = 0; j < resolution + 1; j++)
            {
                float u = j * uv_step;
                float v = i * uv_step;
                uvs.push_back(glm::vec2(u, v));
            }
        }
    }
    // void roll(float angle, glm::vec3 axis) {
    //     // Convertir l'angle en radians
    //     float radians = glm::radians(angle);

    //     // Matrice de rotation
    //     glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, axis);

    //     // Appliquer la rotation à chaque sommet de la sphère
    //     for (int i = 0; i < vertices.size(); ++i) {
    //         vertices[i] = glm::vec3(rotationMatrix * glm::vec4(vertices[i], 1.0f));
    //     }

    //     // Mettre à jour les coordonnées de texture
    //     for (int i = 0; i < uvs.size(); ++i) {
    //         // Convertir les coordonnées de texture en vecteur 3D avec une composante z à 0
    //         glm::vec3 uvVec = glm::vec3(uvs[i], 0.0f);
    //         // Appliquer la rotation à chaque coordonnée de texture
    //         uvVec = glm::vec3(rotationMatrix * glm::vec4(uvVec, 1.0f));
    //         // Mettre à jour les coordonnées de texture avec les nouvelles valeurs
    //         uvs[i] = glm::vec2(uvVec.x, uvVec.y);
    //     }

    //     // Mettre à jour la bounding box après la rotation
    //     glm::vec3 minBounds = glm::vec3(std::numeric_limits<float>::max());
    //     glm::vec3 maxBounds = glm::vec3(std::numeric_limits<float>::lowest());

    //     for (const auto& vertex : vertices) {
    //         minBounds = glm::min(minBounds, vertex);
    //         maxBounds = glm::max(maxBounds, vertex);
    //     }

    //     BoundingBox newBoundingBox(minBounds, maxBounds);
    //     setBoundingBox(newBoundingBox);
    // }

    void roll(float angle, glm::vec3 axis) {
        // Convertir l'angle en radians
        float radians = glm::radians(angle);

        // Matrice de rotation
        glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), radians, axis);

        // Appliquer la rotation à chaque sommet de la sphère
        for (int i = 0; i < vertices.size(); ++i) {
            vertices[i] = glm::vec3(rotationMatrix * glm::vec4(vertices[i], 1.0f));
        }

        // Déplacer les coordonnées de texture circulairement
        for (int i = 0; i < uvs.size(); ++i) {
            uvs[i].x += angle / (2.0f * M_PI);
            // Assurez-vous que la coordonnée de texture reste dans la plage [0, 1]
            if (uvs[i].x > 1.0f) {
                uvs[i].x -= 1.0f;
            }
        }

        // Mettre à jour la bounding box après la rotation
        glm::vec3 minBounds = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 maxBounds = glm::vec3(std::numeric_limits<float>::lowest());

        for (const auto& vertex : vertices) {
            minBounds = glm::min(minBounds, vertex);
            maxBounds = glm::max(maxBounds, vertex);
        }

        BoundingBox newBoundingBox(minBounds, maxBounds);
        setBoundingBox(newBoundingBox);
    }

};
#endif