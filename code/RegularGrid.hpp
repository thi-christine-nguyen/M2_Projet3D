#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <algorithm>
#include <iostream>

#include "variables.hpp"  // Assure-toi d'avoir une variable `programID` pour le shader
#include "BoundingBox.hpp" // Inclure la classe BoundingBox pour l'héritage

class RegularGrid : public BoundingBox {

private:
    GLuint vao = 0;  // Vertex Array Object
    GLuint vbo = 0;  // Vertex Buffer Object
    GLuint ebo = 0;  // Element Buffer Object (facultatif si indices)
    GLuint colorULoc;

    std::vector<glm::vec3> vertices; // Sommets de la grille
    std::vector<GLuint> indices;     // Indices pour dessiner les lignes

    bool initialized = false;

    int resolution;  // Résolution de la grille (nombre de divisions)

public:
    // --- Constructeur ---
    RegularGrid() {}
    RegularGrid(glm::vec3 min, glm::vec3 max, int resolution)
        : BoundingBox(min, max), resolution(resolution) {}

    // --- Accesseurs ---
    GLuint getVAO() const { return vao; }
    size_t getIndicesCount() const { return indices.size(); }

    // --- Méthodes principales ---

    // Initialisation des buffers OpenGL
    void initBuffers() {
        if (initialized) return;

        // Générer les sommets de la grille
        generateVertices();

        // Création des VAO, VBO, et éventuellement EBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // Charger les sommets dans le VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Configurer les attributs de vertex
        glEnableVertexAttribArray(0); // Layout 0 : position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        // Charger les indices dans l'EBO (si nécessaire)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0); // Delink le VAO

        colorULoc = glGetUniformLocation(programID, "color");
        initialized = true;
    }

    void generateVertices() {
        // Calculer la taille de la grille
        glm::vec3 gridSize = getMax() - getMin();
        std::cout << "Grid size: " << gridSize.x << "; " << gridSize.y << "; " << gridSize.z << std::endl;

        // Trouver la taille de voxel uniforme basée sur la résolution
        float voxelSize = std::min(gridSize.x, std::min(gridSize.y, gridSize.z)) / resolution;
        std::cout << "Voxel size: " << voxelSize << std::endl;

        // Ajuster les résolutions pour inclure tout le mesh
        int resolutionX = std::ceil(gridSize.x / voxelSize);
        int resolutionY = std::ceil(gridSize.y / voxelSize);
        int resolutionZ = std::ceil(gridSize.z / voxelSize);

        // Ajuster la taille réelle de la grille pour s'aligner avec les limites
        float adjustedGridX = resolutionX * voxelSize;
        float adjustedGridY = resolutionY * voxelSize;
        float adjustedGridZ = resolutionZ * voxelSize;

        // Calculer les décalages pour centrer la grille si nécessaire
        glm::vec3 minOffset = getMin();
        glm::vec3 maxOffset = minOffset + glm::vec3(adjustedGridX, adjustedGridY, adjustedGridZ);

        std::cout << "Adjusted Grid size: " << adjustedGridX << "; " << adjustedGridY << "; " << adjustedGridZ << std::endl;

        // Générer les sommets de la grille
        vertices.clear();
        indices.clear();

        for (int y = 0; y <= resolutionY; y++) {
            for (int x = 0; x <= resolutionX; x++) {
                for (int z = 0; z <= resolutionZ; z++) {
                    float offsetX = minOffset.x + x * voxelSize;
                    float offsetY = minOffset.y + y * voxelSize;
                    float offsetZ = minOffset.z + z * voxelSize;

                    // Ajouter un sommet
                    vertices.push_back(glm::vec3(offsetX, offsetY, offsetZ));

                    // Connecter les points pour former la grille
                    if (x < resolutionX) {
                        indices.push_back((y * (resolutionX + 1) * (resolutionZ + 1)) + (x * (resolutionZ + 1)) + z);
                        indices.push_back((y * (resolutionX + 1) * (resolutionZ + 1)) + ((x + 1) * (resolutionZ + 1)) + z);
                    }

                    if (y < resolutionY) {
                        indices.push_back((y * (resolutionX + 1) * (resolutionZ + 1)) + (x * (resolutionZ + 1)) + z);
                        indices.push_back(((y + 1) * (resolutionX + 1) * (resolutionZ + 1)) + (x * (resolutionZ + 1)) + z);
                    }

                    if (z < resolutionZ) {
                        indices.push_back((y * (resolutionX + 1) * (resolutionZ + 1)) + (x * (resolutionZ + 1)) + z);
                        indices.push_back((y * (resolutionX + 1) * (resolutionZ + 1)) + (x * (resolutionZ + 1)) + (z + 1));
                    }
                }
            }
        }

        std::cout << "Total vertices: " << vertices.size() << ", Total indices: " << indices.size() << std::endl;
    }

    // Dessiner la grille
    void draw() const {
        if (!initialized) {
            std::cerr << "RegularGrid not initialized! Call initBuffers() first." << std::endl;
            return;
        }

        // Activer le VAO de la grille
        glBindVertexArray(vao);
        glm::vec4 color {0.f, 1.f, 0.f, 1.f};  // Vert par défaut
        glUniform4fv(colorULoc, 1, &color[0]); // Couleur
        glLineWidth(1.f);

        // Dessiner les lignes
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }

    // Destructeur
    ~RegularGrid() {
        if (initialized) {
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
            glDeleteVertexArrays(1, &vao);
        }
    }
};

#endif
