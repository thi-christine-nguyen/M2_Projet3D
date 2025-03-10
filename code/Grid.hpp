#ifndef GRID_HPP__
#define GRID_HPP__

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <glm/gtx/string_cast.hpp>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <set>
#include "MarchingCubesTable.hpp"

const float LITTLE_EPSILON = 1e-6f;
const float EPSILON = 1e-4f;
const float BIG_EPSILON = 1e-2f;

enum class VoxelizationMethod {
    Simple,      // Voxelisation complète (avec intérieur)
    Optimized,      // Voxelisation complète (avec intérieur) optimisé sur les axes
    Surface    // Voxelisation de la surface uniquement
};

struct VoxelData {
    glm::vec3 center;   // Centre du voxel
    float halfSize;     // Moitié de la taille du voxel
    int isEmpty;
    int isSelected;
    glm::vec3 isEmptyOnAxe;
    std::array<int, 8> edge = {0, 0, 0, 0, 0, 0, 0, 0};

    VoxelData() {}

    VoxelData(const glm::vec3& c, float hs, int ie, int is)
        : center(c), halfSize(hs), isEmpty(ie), isSelected(is) {}
};

namespace std {
    template <>
    struct less<glm::vec3> {
        bool operator()(const glm::vec3& a, const glm::vec3& b) const {
            // Comparer d'abord sur l'axe X, puis Y, puis Z si nécessaire
            if (a.x != b.x) return a.x < b.x;
            if (a.y != b.y) return a.y < b.y;
            return a.z < b.z;
        }
    };
}

class Grid {
protected:
    glm::vec3 minBounds; // Coordonnées minimales
    glm::vec3 maxBounds; // Coordonnées maximales

    int resolution;      // Résolution de la grille
    VoxelizationMethod method;

    std::vector<VoxelData> voxels; // Liste des voxels
    GLuint VAO, VBO;           // Buffers OpenGL pour les voxels
    glm::vec3 color {1.f, 1.f, 1.f};

    std::vector<glm::vec3> activeCorner; 
    VoxelData *selectedVoxel;

public:
    Grid() {};
    Grid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method)
        : minBounds(minBounds), maxBounds(maxBounds), resolution(resolution), method(method) {
        }

    void initializeBuffers();    // Prépare les buffers OpenGL

    void printGrid() const;
    void draw(GLuint shaderID, glm::mat4 transformMat = glm::mat4(1.0f)); // Rendu des voxels via un shader
    bool triangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                         const glm::vec3& boxCenter, const glm::vec3& boxHalfSize) const;
    bool testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
                           const glm::vec3& boxHalfSize) const;
    void setColor(glm::vec3 c);
    virtual void update(float deltaTime, GLFWwindow* window) {
        std::cerr << "Marching Cubes not implemented." << std::endl;
    }

    virtual void marchingCube( std::vector<unsigned short> &indices, std::vector<glm::vec3> &vertices) {
        std::cerr << "Marching Cubes not implemented." << std::endl;
    }
    void removeDuplicates(std::vector<glm::vec3>& activeCorner);
    void createOffFile(std::vector<unsigned short> &indices, std::vector<glm::vec3> &vertices, std::string& filename);

    virtual ~Grid() = default;
};

#endif
