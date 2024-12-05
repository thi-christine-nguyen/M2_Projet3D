#include "RegularGrid.hpp"
#include <iostream>

RegularGrid::RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution = 10)
    : minBounds(minBounds), maxBounds(maxBounds), resolution(resolution), VAO(0), VBO(0) {}

RegularGrid::RegularGrid(const std::vector<glm::vec3>& vertices, int resolution = 10)
{
    this->resolution = resolution;
    init(vertices);
}

void RegularGrid::init(const std::vector<glm::vec3>& vertices) {
    if (vertices.empty()) return;

    glm::vec3 minVertex = vertices[0];
    glm::vec3 maxVertex = vertices[0];

    for (const auto& vertex : vertices) {
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }

    minBounds = minVertex;
    maxBounds = maxVertex;

    // Régénérer les sommets et indices
    generateVoxels();
    initializeBuffers();
}

void RegularGrid::generateVoxels() {
     // Dimensions de la grille
    glm::vec3 gridSize = maxBounds - minBounds;

    // Calculer la taille réelle d'un voxel cubique
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

    // Calculer les nouvelles résolutions pour couvrir complètement la grille
    int resolutionX = std::ceil(gridSize.x / voxelSize);
    int resolutionY = std::ceil(gridSize.y / voxelSize);
    int resolutionZ = std::ceil(gridSize.z / voxelSize);
    float halfSize = voxelSize / 2;
    std::cout << "Voxel size (cubique): " << voxelSize << ", halfSize: " << halfSize << std::endl;
    std::cout << "Resolutions adjusted: X=" << resolutionX << ", Y=" << resolutionY << ", Z=" << resolutionZ << std::endl;

    voxels.clear();

    for (int x = 0; x < resolutionX; ++x) {
        for (int y = 0; y < resolutionY; ++y) {
            for (int z = 0; z < resolutionZ; ++z) {
                glm::vec3 center = minBounds + glm::vec3(x, y, z) * voxelSize + glm::vec3(halfSize);
                voxels.emplace_back(center, halfSize);
            }
        }
    }

    std::cout << "Generated " << voxels.size() << " voxels.\n";
}

void RegularGrid::initializeBuffers() {
    // if (VAO != 0) return; // Éviter une double initialisation

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Préparer les données des voxels
    std::vector<VoxelData> bufferData;
    for (const auto& voxel : voxels) {
        bufferData.emplace_back(voxel.center, voxel.halfSize);
        // std::cout << "center={" << bufferData.back().center.x << ", " << bufferData.back().center.y << ", " << bufferData.back().center.z << "} halfSize=" << bufferData.back().halfSize << std::endl;
    }
    // std::cout << bufferData.size() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(VoxelData), bufferData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)offsetof(VoxelData, center));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)offsetof(VoxelData, halfSize));

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RegularGrid::draw(GLuint shaderID, glm::mat4 transformMat) {
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // std::cout << shaderID << std::endl;
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &transformMat[0][0]); // Matrice de transformation
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, voxels.size());

    glBindVertexArray(0);
}

RegularGrid::~RegularGrid() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
}
