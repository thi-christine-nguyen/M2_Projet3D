#ifndef VOXEL_HPP__
#define VOXEL_HPP__

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>  // Inclure GLEW ou votre bibliothèque OpenGL
#include "variables.hpp"

class Voxel {
public:
    glm::vec3 center;  // Centre du voxel
    float size;        // Taille uniforme (côté du cube)
    bool isEmpty;      // Indique si le voxel est vide ou contient une partie du mesh

    GLuint VAO;        // Vertex Array Object
    GLuint VBO;        // Vertex Buffer Object
    GLuint EBO;        // Element Buffer Object (pour les indices)
    std::vector<glm::vec3> vertices; // Sommets du voxel (8 sommets)
    std::vector<unsigned int> indices; // Indices pour former les triangles

    // --- Constructeurs ---
    Voxel() : center(0.0f), size(1.0f), isEmpty(true), VAO(0), VBO(0), EBO(0) {}
    Voxel(glm::vec3 center, float size) : center(center), size(size), isEmpty(true), VAO(0), VBO(0), EBO(0) {
        generateVertices(); // Génération des sommets et indices au moment de la création
        initializeBuffers(); // Création des buffers OpenGL
    }

    // --- Génération des sommets et indices ---
    void generateVertices() {
        glm::vec3 halfSize(size * 0.5f);

        // Définir les 8 sommets du cube
        vertices = {
            center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z), // v0
            center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z),  // v1
            center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z),   // v2
            center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z),  // v3
            center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z),  // v4
            center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z),   // v5
            center + glm::vec3(halfSize.x, halfSize.y, halfSize.z),    // v6
            center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z)    // v7
        };

        // Définir les indices pour former les triangles (2 triangles par face)
        indices = {
            // Face arrière (-Z)
            0, 1, 2,
            0, 2, 3,

            // Face avant (+Z)
            4, 5, 6,
            4, 6, 7,

            // Face gauche (-X)
            0, 3, 7,
            0, 7, 4,

            // Face droite (+X)
            1, 2, 6,
            1, 6, 5,

            // Face inférieure (-Y)
            0, 1, 5,
            0, 5, 4,

            // Face supérieure (+Y)
            3, 2, 6,
            3, 6, 7
        };
    }

    // --- Initialisation des buffers OpenGL ---
    void initializeBuffers() {
        if (VAO != 0) return; // Empêcher la réinitialisation

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        std::cout << "VAO ID: " << VAO << ", VBO ID: " << VBO << ", EBO ID: " << EBO << std::endl;
        glBindVertexArray(VAO);

        // Charger les sommets dans le VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Charger les indices dans l'EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Configurer les attributs de vertex
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0); // Activer l'attribut 0 (position)

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // --- Rendu du voxel ---
    void draw() const {
        if (isEmpty) return; // Ne pas dessiner les voxels vides

        glBindVertexArray(VAO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // Lier l'EBO
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        // std::cout << "Rendering voxel at: " << center.x << "; " << center.y << "; " << center.z << std::endl;
        glBindVertexArray(0);
    }

    // --- Destructeur ---
    ~Voxel() {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
        if (EBO != 0) {
            glDeleteBuffers(1, &EBO);
            EBO = 0;
        }
    }
};

#endif
