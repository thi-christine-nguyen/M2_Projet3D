#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include "Grid.hpp"

class RegularGrid : public Grid {
private:
    int gridResolutionX;
    int gridResolutionY;
    int gridResolutionZ;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

public:
    RegularGrid() {};
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method);
    RegularGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution, VoxelizationMethod method);

    void generateVoxels();       // Génère les voxels dans la grille
   
    VoxelData getVoxel(int x, int y, int z);
    int getColumnIndex(const VoxelData& voxel, int projectionAxis) const;
    int getVoxelIndex(int x, int y, int z) const;
    bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    void processRaycastingForAxis(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int projectionAxis);
   
    void printGrid() const;
    void init(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, VoxelizationMethod method);

    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void voxelizeMeshSurface(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void marchingCube() override;
    bool areVec3Equal(const glm::vec3& v1, const glm::vec3& v2, float epsilon = 1e-6f) {
        return glm::all(glm::lessThan(glm::abs(v1 - v2), glm::vec3(epsilon)));
    }
    bool isCornerActive(const glm::vec3& corner, const std::vector<glm::vec3>& activeCorner, float epsilon = 1e-6f) {
        for (const auto& active : activeCorner) {
            if (areVec3Equal(corner, active, epsilon)) {
                return true;
            }
        }
        return false;
    }

    bool isVoxelOnBorder(const VoxelData &voxel) {
        glm::vec3 voxelPos = voxel.center;
        glm::vec3 gridMin = glm::vec3(0.0f); // Position minimale de la grille
        glm::vec3 gridMax = glm::vec3(gridResolutionX, gridResolutionY, gridResolutionZ); // Position maximale de la grille
        
        // Vérifier si le voxel est sur un bord (face ou coin)
        if (voxelPos.x <= gridMin.x || voxelPos.x >= gridMax.x ||
            voxelPos.y <= gridMin.y || voxelPos.y >= gridMax.y ||
            voxelPos.z <= gridMin.z || voxelPos.z >= gridMax.z) {
            return true;
        }
        return false;
    }





    virtual ~RegularGrid() = default;
};

#endif
