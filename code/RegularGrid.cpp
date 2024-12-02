#include "RegularGrid.hpp"
#include <iostream>

RegularGrid::RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution)
    : minBounds(minBounds), maxBounds(maxBounds), resolution(resolution), VAO(0), VBO(0) {}

void RegularGrid::generateVoxels() {
    glm::vec3 gridSize = maxBounds - minBounds;
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

    voxels.clear();

    for (int x = 0; x < resolution; ++x) {
        for (int y = 0; y < resolution; ++y) {
            for (int z = 0; z < resolution; ++z) {
                glm::vec3 center = minBounds + glm::vec3(x, y, z) * voxelSize + glm::vec3(voxelSize * 0.5f);
                voxels.emplace_back(center, voxelSize, false); // Par défaut, tous les voxels sont pleins
            }
        }
    }

    std::cout << "Generated " << voxels.size() << " voxels.\n";
}

void RegularGrid::initializeBuffers() {
    if (VAO != 0) return; // Éviter une double initialisation

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Préparer les positions des voxels
    std::vector<glm::vec3> voxelPositions;
    for (const auto& voxel : voxels) {
        if (!voxel.isEmpty) {
            voxelPositions.push_back(voxel.center);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, voxelPositions.size() * sizeof(glm::vec3), voxelPositions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RegularGrid::render(GLuint shaderID) {
    glUseProgram(shaderID);
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(voxels.size()));
    glBindVertexArray(0);
}

RegularGrid::~RegularGrid() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}
