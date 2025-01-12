#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include "Grid.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <algorithm>

class RegularGrid : public Grid {
private:
    int gridResolutionX;
    int gridResolutionY;
    int gridResolutionZ;

    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    bool keyXUpPressed = false;
    bool keyXDownPressed = false;
    bool keyYUpPressed = false;
    bool keyYDownPressed = false;
    bool keyZUpPressed = false;
    bool keyZDownPressed = false;
    bool keyAddPressed = false;
    bool keyDeletePressed = false;
public:
    RegularGrid() {};
    RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method);
    RegularGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution, VoxelizationMethod method);

    void generateVoxels();       // Génère les voxels dans la grille
    void update(float deltaTime, GLFWwindow* window) override;

    VoxelData getVoxel(int x, int y, int z);
    int getColumnIndex(const VoxelData& voxel, int projectionAxis) const;
    int getVoxelIndex(int x, int y, int z) const;
    int getVoxelIndex(const VoxelData&voxel) const;
    glm::vec3 getVoxelVec3Index(const VoxelData&voxel) const;
    bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t);
    void processRaycastingForAxis(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int projectionAxis);
   
    void printGrid() const;
    void init(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, VoxelizationMethod method);

    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void voxelizeMeshSurface(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void marchingCube( std::vector<unsigned short> &indices, std::vector<glm::vec3> &vertices) override;

    virtual ~RegularGrid() = default;
};

#endif