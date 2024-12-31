#include "AdaptativeGrid.hpp"
#include <iostream>

AdaptativeGrid::AdaptativeGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution = 10, VoxelizationMethod method = VoxelizationMethod::Optimized)
    : Grid(minBounds, maxBounds, resolution, method), VAO(0), VBO(0)
{
    root = std::make_unique<OctreeNode>(minBounds, maxBounds);
    initializeBuffers();
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
    initializeBuffers();
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

void AdaptativeGrid::initializeBuffers() {
    // if (VAO != 0) return; // Éviter une double initialisation

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

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

bool AdaptativeGrid::triangleIntersectsAABB(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
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
bool AdaptativeGrid::testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
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

void AdaptativeGrid::draw(GLuint shaderID, glm::mat4 transformMat) {
    // std::cout << shaderID << std::endl;
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "model"), 1, GL_FALSE, &transformMat[0][0]); // Matrice de transformation
    glUniform3f(glGetUniformLocation(shaderID, "objectColor"), 1.0, 1.0, 1.0); 
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, voxels.size());

    glBindVertexArray(0);
}
