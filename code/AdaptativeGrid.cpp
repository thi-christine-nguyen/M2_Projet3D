#include "AdaptativeGrid.hpp"
#include <iostream>

AdaptativeGrid::AdaptativeGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution = 10, VoxelizationMethod method = VoxelizationMethod::Optimized)
    : Grid(minBounds, maxBounds, resolution, method)
{
    root = std::make_unique<OctreeNode>(minBounds, maxBounds);
    Grid::initializeBuffers();
}

AdaptativeGrid::AdaptativeGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution = 10, VoxelizationMethod method = VoxelizationMethod::Optimized)
{
    if (vertices.empty()) return;
    this->resolution = resolution;

    glm::vec3 minVertex = vertices[0];
    glm::vec3 maxVertex = vertices[0];

    for (const auto& vertex : vertices) {
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }

    minBounds = minVertex;
    maxBounds = maxVertex;

    root = std::make_unique<OctreeNode>(minBounds, maxBounds);
    voxelizeMesh(indices, vertices);
    root->print();
    printGrid();
    Grid::initializeBuffers();
}

void AdaptativeGrid::fillVoxelDataRecursive(const OctreeNode& node) {
    if (node.isLeaf) {
        // Créez un VoxelData pour chaque nœud feuille
        glm::vec3 center = (node.minBounds + node.maxBounds) * 0.5f;
        glm::vec3 size = (node.maxBounds - node.minBounds) * 0.5f;
        // std::cout << "Node is Leaf: center(" 
        //     << center.x << ", " 
        //     << center.y << ", " 
        //     << center.z << "), size(" 
        //     << size.x << ", " 
        //     << size.y << ", " 
        //     << size.z << ")" << std::endl;
        voxels.emplace_back(VoxelData{center, size.x, 0}); // 0 si le nœud est rempli
    } else {
        // Parcourez les enfants si ce n'est pas une feuille
        for (const auto& child : node.children) {
            fillVoxelDataRecursive(child);
        }
    }
}

void AdaptativeGrid::voxelizeNode(OctreeNode& node, const std::vector<unsigned short>& indices,
                    const std::vector<glm::vec3>& vertices, int depth) {
    if (depth == 0 || node.isLeaf == false) return;
    bool intersected = false;

    // Vérifier les intersections entre les triangles et le nœud
    for (size_t i = 0; i < indices.size(); i += 3) {
        const glm::vec3& v0 = vertices[indices[i]];
        const glm::vec3& v1 = vertices[indices[i + 1]];
        const glm::vec3& v2 = vertices[indices[i + 2]];

        if (node.intersectsTriangle(v0, v1, v2)) {
            // node.isLeaf = false;
            intersected = true;
            break;
        }
    }

    // if (node.isLeaf) return;
    if (!intersected) {
        node.isLeaf = false;
        return;
    }

    // Subdiviser si le nœud n'est pas une feuille
    if (depth > 1) {
        node.subdivide();
        for (auto& child : node.children) {
            voxelizeNode(child, indices, vertices, depth - 1);
        }
    }
}

void AdaptativeGrid::voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices) {
    voxelizeNode(*root, indices, vertices, resolution);
    voxels.clear();
    fillVoxelDataRecursive(*root);
}

void AdaptativeGrid::printGrid() const {
    // Dimensions de la grille
    glm::vec3 gridSize = maxBounds - minBounds;
    // Calculer la taille réelle d'un voxel cubique
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});
    // Calculer les résolutions ajustées pour chaque dimension
    int resolutionX = std::ceil(gridSize.x / voxelSize);
    int resolutionY = std::ceil(gridSize.y / voxelSize);
    int resolutionZ = std::ceil(gridSize.z / voxelSize);
    // Afficher la grille sous forme de 1 et 0
    std::cout << "Voxel Grid (1 = filled, 0 = empty):\n";
    // Parcourir chaque voxel
    for (const auto& voxel : voxels) {
        std::cout << "Voxel Center: (" 
                << voxel.center.x << ", " 
                << voxel.center.y << ", " 
                << voxel.center.z << "), Half-Size: " 
                << voxel.halfSize 
                << ", Is Empty: " << voxel.isEmpty << std::endl;
    }
}

