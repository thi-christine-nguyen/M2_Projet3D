#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Voxel.hpp"

class RegularGrid {
private:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales
    int resolution;      // Résolution de la grille

    std::vector<Voxel> voxels; // Liste des voxels
    GLuint VAO, VBO;           // Buffers OpenGL pour les voxels

public:
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution);

    void generateVoxels();       // Génère les voxels dans la grille
    void initializeBuffers();    // Prépare les buffers OpenGL
    void render(GLuint shaderID); // Rendu des voxels via un shader

    ~RegularGrid();
};

#endif
