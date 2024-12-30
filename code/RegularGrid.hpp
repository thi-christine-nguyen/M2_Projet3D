#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

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
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method);
    RegularGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution, VoxelizationMethod method);

    void generateVoxels();       // Génère les voxels dans la grille
    void initializeBuffers();    // Prépare les buffers OpenGL
    VoxelData getVoxel(int x, int y, int z);
    int getColumnIndex(const VoxelData& voxel, int projectionAxis) const;
    int getVoxelIndex(int x, int y, int z) const;
    bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    void processRaycastingForAxis(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int projectionAxis);
    bool triangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                         const glm::vec3& boxCenter, const glm::vec3& boxHalfSize) const;
    bool testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
                           const glm::vec3& boxHalfSize) const;

    void printGrid() const;
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    void init(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, VoxelizationMethod method);

    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void voxelizeMeshSurface(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);

    virtual ~RegularGrid() = default;
};

#endif
