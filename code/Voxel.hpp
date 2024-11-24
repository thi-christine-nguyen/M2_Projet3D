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

    // --- Constructeurs ---
    Voxel() : center(0.0f), size(1.0f), isEmpty(true), VAO(0), VBO(0) {}
    Voxel(glm::vec3 center, float size) : center(center), size(size), isEmpty(true), VAO(0), VBO(0) {}

    ~Voxel() {
        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &VAO);
    }

    // --- Génération des sommets du voxel ---
    void getVertices(std::vector<glm::vec3>& vertices) const {
        glm::vec3 halfSize(size * 0.5f);

        // Calculer les 8 sommets du cube
        glm::vec3 v0 = center + glm::vec3(-halfSize.x, -halfSize.y, -halfSize.z);
        glm::vec3 v1 = center + glm::vec3(halfSize.x, -halfSize.y, -halfSize.z);
        glm::vec3 v2 = center + glm::vec3(halfSize.x, halfSize.y, -halfSize.z);
        glm::vec3 v3 = center + glm::vec3(-halfSize.x, halfSize.y, -halfSize.z);

        glm::vec3 v4 = center + glm::vec3(-halfSize.x, -halfSize.y, halfSize.z);
        glm::vec3 v5 = center + glm::vec3(halfSize.x, -halfSize.y, halfSize.z);
        glm::vec3 v6 = center + glm::vec3(halfSize.x, halfSize.y, halfSize.z);
        glm::vec3 v7 = center + glm::vec3(-halfSize.x, halfSize.y, halfSize.z);

        // Ajouter les sommets dans l'ordre des lignes (wireframe)
        vertices.insert(vertices.end(), {v0, v1, v1, v2, v2, v3, v3, v0,
                                         v4, v5, v5, v6, v6, v7, v7, v4,
                                         v0, v4, v1, v5, v2, v6, v3, v7});
    }

    // --- Initialisation des buffers OpenGL ---
    void initializeBuffers() {
        std::vector<glm::vec3> vertices;
        getVertices(vertices);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // Activer l'attribut 0
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // --- Rendu du voxel ---
    void render() const {
        if (isEmpty) return;

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 24);  // 24 points pour un cube en wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glBindVertexArray(0);
    }
};

#endif
