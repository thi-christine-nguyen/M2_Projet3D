#ifndef PLANE_HPP__
#define PLANE_HPP__
#include "TP1/GameObject.hpp"

class Plane : public GameObject {
public:
    int resolution;
    int size;
    bool randomHeight;

    // Constructeur de Plane
    Plane() {}

    Plane(std::string name, unsigned int res, unsigned int size, int textureID, const char *texturePath, GLuint programID = 0, bool randomHeight=false) {
        this->name = name;
        this->resolution = res;
        this->size = size;
        this->type = PLANE;
        this->textureID = textureID;
        this->texturePath = texturePath;
        this->randomHeight = randomHeight;

        // Init le plan
        init();
        // Generer et charger les buffers
        this->GenerateBuffers(programID);
        // Init la bounding box
        this->initBoundingBox();
    }


    void init()
    {
        indices.clear();
        // triangles.clear();
        vertices.clear();
        uvs.clear();

        int nbVertices = resolution * resolution;
        float step = size / (float)resolution;
        float x, y, z, u, v;

        for (int i = 0; i <= resolution; i++)
        {
            for (int j = 0; j <= resolution; j++)
            {
                x = j * step;
                if (randomHeight)
                {
                    y = std::max((float)rand() / (RAND_MAX), 0.f);
                }
                else
                {
                    y = 0;
                }
                z = i * step;
                u = x / size;
                v = z / size;
                vertices.push_back(glm::vec3(x - size / 2.f, y, z - size / 2.f));
                uvs.push_back(glm::vec2(u, v));
            }
        }

        for (int i = 0; i < resolution; i++)
        {
            for (int j = 0; j < resolution; j++)
            {
                unsigned short bottomLeft = j + i * (resolution + 1);
                unsigned short bottomRight = bottomLeft + 1;
                unsigned short topLeft = bottomLeft + (resolution + 1);
                unsigned short topRight = topLeft + 1;

                // triangles.push_back({bottomLeft, bottomRight, topLeft});
                // triangles.push_back({bottomRight, topRight, topLeft});

                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
                indices.push_back(topLeft);
                indices.push_back(bottomRight);
                indices.push_back(topRight);
                indices.push_back(topLeft);
            }
        }
        std::cout << "indices size: " << indices.size() << std::endl;
        // std::cout << "triangles size: " << triangles.size() << std::endl;
        std::cout << "vertices size: " << vertices.size() << std::endl;
        std::cout << "uvs size: " << uvs.size() << std::endl;
    }

    void computeUV() 
    {
        uvs.clear();

        float UvStep = 1.0f / (float)resolution;
        for (int i = 0; i <= resolution; i++)
        {
            for (int j = 0; j <= resolution; j++)
            {
                float u = j * UvStep;
                float v = i * UvStep;
                uvs.push_back(glm::vec2(u, v));
            }
        }
    }

    // Test mode édition du terrain
    // glm::vec3 getMousePositionOnPlane(GLFWwindow* window) {
    //     double mouseX, mouseY;
    //     glfwGetCursorPos(window, &mouseX, &mouseY);

    //     // Récupérer les dimensions de la fenêtre
    //     int windowWidth, windowHeight;
    //     glfwGetWindowSize(window, &windowWidth, &windowHeight);

    //     // Coordonnées normalisées de la souris dans la fenêtre (-1,1)
    //     float normalizedX = (2.0f * mouseX) / windowWidth - 1.0f;
    //     float normalizedY = 1.0f - (2.0f * mouseY) / windowHeight;

    //     // Créer un rayon depuis la caméra
    //     glm::vec4 rayClip(normalizedX, normalizedY, -1.0f, 1.0f);
    //     glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    //     rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

    //     // Transformer le rayon en espace du monde
    //     glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    //     rayWorld = glm::normalize(rayWorld);

    //     // Intersection du rayon avec le plan
    //     float distance = -cameraPosition.y / rayWorld.y;
    //     return cameraPosition + (rayWorld * distance);
    // }
};

#endif