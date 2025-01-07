#include "Grid.hpp"
#include <iostream>

void Grid::initializeBuffers() {
    // if (VAO != 0) return; // Éviter une double initialisation

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);using Voxel = std::tuple<int, int, int>;

    glBindVertexArray(VAO);

    // Préparer les données des voxels
    std::vector<VoxelData> bufferData;
    for (const auto& voxel : voxels) {
        bufferData.emplace_back(voxel.center, voxel.halfSize, voxel.isEmpty);
        // std::cout << voxel.isEmpty << std::endl;
        // std::cout << "center={" << bufferData.back().center.x << ", " << bufferData.back().center.y << ", " << bufferData.back().center.z << "} halfSize=" << bufferData.back().halfSize << std::endl;
    }
    // std::cout << bufferData.size() << std::endl;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, bufferData.size() * sizeof(VoxelData), bufferData.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)offsetof(VoxelData, center));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(VoxelData), (void*)offsetof(VoxelData, halfSize));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(VoxelData), (void*)offsetof(VoxelData, isEmpty));

    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

bool Grid::triangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
                                         const glm::vec3& boxCenter, const glm::vec3& boxHalfSize) const {
    // Étape 1 : Translation des points du triangle vers le centre de l'AABB
    glm::vec3 t0 = v0 - boxCenter;
    glm::vec3 t1 = v1 - boxCenter;
    glm::vec3 t2 = v2 - boxCenter;

    // Étape 2 : Axes de la boîte
    const glm::vec3 boxAxes[3] = { glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1) };

    // Étape 3 : Test sur les axes de la boîte
    for (int i = 0; i < 3; ++i) {
        float r = boxHalfSize[i];
        float p0 = t0[i], p1 = t1[i], p2 = t2[i];
        float minP = glm::min(p0, glm::min(p1, p2));
        float maxP = glm::max(p0, glm::max(p1, p2));
        if (minP > r || maxP < -r) return false; // Pas d'intersection
    }

    // Étape 4 : Normale du triangle
    glm::vec3 triangleNormal = glm::cross(t1 - t0, t2 - t0);
    if (!testAxis(triangleNormal, t0, t1, t2, boxHalfSize)) return false;

    // Étape 5 : Axes croisés entre les arêtes du triangle et les axes de la boîte
    const glm::vec3 triangleEdges[3] = { t1 - t0, t2 - t1, t0 - t2 };
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            glm::vec3 axis = glm::cross(triangleEdges[i], boxAxes[j]);
            if (!testAxis(axis, t0, t1, t2, boxHalfSize)) return false;
        }
    }

    return true; // Pas d'axe de séparation trouvé, intersection existante
}

// Méthode utilitaire pour tester un axe de séparation
bool Grid::testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
                           const glm::vec3& boxHalfSize) const {
    if (glm::dot(axis, axis) < 1e-6f) return true; // Vérification de la quasi-nullité de l'axe

    // Projeter le triangle sur l'axe
    float p0 = glm::dot(t0, axis);
    float p1 = glm::dot(t1, axis);
    float p2 = glm::dot(t2, axis);
    float triMin = glm::min(p0, glm::min(p1, p2));
    float triMax = glm::max(p0, glm::max(p1, p2));

    // Projeter l'AABB sur l'axe
    glm::vec3 absAxis = glm::abs(axis);
    float boxRadius = glm::dot(boxHalfSize, absAxis);

    // Tester la séparation
    return !(triMin > boxRadius || triMax < -boxRadius);
}

void Grid::draw(GLuint shaderID, glm::mat4 transformMat) {
    // std::cout << shaderID << std::endl;
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &transformMat[0][0]); // Matrice de transformation
    glUniform3fv(glGetUniformLocation(shaderID, "objectColor"), 1, &color[0]); // Couleur
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, voxels.size());
    glBindVertexArray(0);
}

void Grid::setColor(glm::vec3 c){
    color = c; 
}


