#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <algorithm>

struct VoxelData {
    glm::vec3 center;   // Centre du voxel
    float halfSize;     // Moitié de la taille du voxel
    int isEmpty;

    VoxelData(const glm::vec3& c, float hs, bool ie)
        : center(c), halfSize(hs), isEmpty(ie) {}
};

class RegularGrid {
private:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales

    int resolution;      // Résolution de la grille
    int gridResolutionX;
    int gridResolutionY;
    int gridResolutionZ;

    std::vector<VoxelData> voxels; // Liste des voxels
    GLuint VAO, VBO;           // Buffers OpenGL pour les voxels


public:
    RegularGrid() {};
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution);
    RegularGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution);

    void generateVoxels();       // Génère les voxels dans la grille
    void initializeBuffers();    // Prépare les buffers OpenGL
    VoxelData getVoxel(int x, int y, int z);
    int getColumnIndex(const VoxelData& voxel, int projectionAxis) const;
    bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void printGrid() const;
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    void init(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);

    ~RegularGrid();
};

#endif
