#ifndef GRID_HPP__
#define GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>

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

    VoxelData(const glm::vec3& c, float hs, bool ie)
        : center(c), halfSize(hs), isEmpty(ie) {}
};

class Grid {
protected:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales

    int resolution;      // Résolution de la grille
    VoxelizationMethod method;

public:
    Grid() {};
    Grid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method)
        : minBounds(minBounds), maxBounds(maxBounds), resolution(resolution), method(method) {}

    void printGrid() const;
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    virtual ~Grid() = default;
};

#endif
