#ifndef GRID_HPP__
#define GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include "MarchingCubeTable.hpp"

enum class VoxelizationMethod {
    Simple,      // Voxelisation complète (avec intérieur)
    Optimized,      // Voxelisation complète (avec intérieur) optimisé sur les axes
    Surface    // Voxelisation de la surface uniquement
};

struct VoxelData {
    glm::vec3 center;   // Centre du voxel
    float halfSize;     // Moitié de la taille du voxel
    int isEmpty;
    glm::vec3 isEmptyOnAxe;
    std::array<int, 8> edge;

    VoxelData(const glm::vec3& c, float hs, bool ie)
        : center(c), halfSize(hs), isEmpty(ie) {}
};

class Grid {
protected:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales

    int resolution;      // Résolution de la grille
    VoxelizationMethod method;

    std::vector<VoxelData> voxels; // Liste des voxels
    GLuint VAO, VBO;           // Buffers OpenGL pour les voxels
    glm::vec3 color {1.f, 1.f, 1.f};

public:
    Grid() {};
    Grid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method)
        : minBounds(minBounds), maxBounds(maxBounds), resolution(resolution), method(method) {}

    void initializeBuffers();    // Prépare les buffers OpenGL

    void printGrid() const;
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    bool triangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                         const glm::vec3& boxCenter, const glm::vec3& boxHalfSize) const;
    bool testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
                           const glm::vec3& boxHalfSize) const;
    void setColor(glm::vec3 c);

    virtual void marchingCube() {
        std::cerr << "Marching Cubes not implemented." << std::endl;
    }


    virtual ~Grid() = default;
};

#endif
