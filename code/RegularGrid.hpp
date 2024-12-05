#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>

struct VoxelData {
    glm::vec3 center;   // Centre du voxel
    float halfSize;     // Moitié de la taille du voxel

    VoxelData(const glm::vec3& c, float hs)
        : center(c), halfSize(hs) {}
};

class RegularGrid {
private:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales
    int resolution;      // Résolution de la grille

    std::vector<VoxelData> voxels; // Liste des voxels
    GLuint VAO, VBO;           // Buffers OpenGL pour les voxels


public:
    RegularGrid() {};
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution);
    RegularGrid(const std::vector<glm::vec3>& vertices, int resolution);

    void generateVoxels();       // Génère les voxels dans la grille
    void initializeBuffers();    // Prépare les buffers OpenGL
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    void init(const std::vector<glm::vec3>& vertices);

    ~RegularGrid();
};

#endif
