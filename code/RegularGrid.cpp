#include "RegularGrid.hpp"
#include <iostream>

RegularGrid::RegularGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution = 10, VoxelizationMethod method = VoxelizationMethod::Optimized)
    : Grid(minBounds, maxBounds, resolution, method){}

RegularGrid::RegularGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution = 10, VoxelizationMethod method = VoxelizationMethod::Optimized)
{
    this->resolution = resolution;
    init(indices, vertices, method);
}

void RegularGrid::init(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, VoxelizationMethod method) {
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

    // Initialiser selon la méthode choisie
    switch (method) {
        case VoxelizationMethod::Simple:
            std::cout << "Using Simple voxelization.\n";
            voxelizeMesh(indices, vertices);
            break;
        case VoxelizationMethod::Optimized:
            std::cout << "Using Optimized voxelization on axes.\n";
            optimizedVoxelizeMesh(indices, vertices);
            break;
        case VoxelizationMethod::Surface:
            std::cout << "Using surface voxelization.\n";
            voxelizeMeshSurface(indices, vertices);
            break;
    }
    selectedVoxel = &voxels[getVoxelIndex(0, 0, gridResolutionZ - 1)];
    selectedVoxel->isSelected = true;
    Grid::initializeBuffers();
}

void RegularGrid::generateVoxels() {
    // Dimensions de la grille
    glm::vec3 gridSize = maxBounds - minBounds;

    // Calculer la taille réelle d'un voxel cubique
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

    // Calculer les résolutions de la grille, en ajoutant des voxels au bord
    gridResolutionX = std::ceil(gridSize.x / voxelSize);  // +2 pour ajouter des voxels au bord
    gridResolutionY = std::ceil(gridSize.y / voxelSize);  // +2 pour ajouter des voxels au bord
    gridResolutionZ = std::ceil(gridSize.z / voxelSize);  // +2 pour ajouter des voxels au bord
    
    float halfSize = voxelSize / 2;
    
    std::cout << "Voxel size (cubique): " << voxelSize << ", halfSize: " << halfSize << std::endl;
    std::cout << "Resolutions adjusted: X=" << gridResolutionX << ", Y=" << gridResolutionY << ", Z=" << gridResolutionZ << std::endl;

    voxels.clear();

    // Créer les voxels en tenant compte des nouveaux bords
    for (int x = 0; x < gridResolutionX; ++x) {
        for (int y = 0; y < gridResolutionY; ++y) {
            for (int z = 0; z < gridResolutionZ; ++z) {
                // Décaler les positions des voxels pour englober tous les voxels
                glm::vec3 center = minBounds + glm::vec3(x, y, z) * voxelSize + glm::vec3(halfSize);
                voxels.emplace_back(center, halfSize, 1, 0);
            }
        }
    }

    std::cout << "Generated " << voxels.size() << " voxels.\n";
}

VoxelData RegularGrid::getVoxel(int x, int y, int z) {
    // Calculer l'index unique dans la liste des voxels
    int index = x * gridResolutionY * gridResolutionZ + y * gridResolutionZ + z;
    return voxels[index]; // Retourner le centre déjà stocké
}

int RegularGrid::getColumnIndex(const VoxelData& voxel, int projectionAxis) const {
    // Identifiants pour les dimensions secondaires
    int index1, index2;

    if (projectionAxis == 0) {       // Rayon le long de X
        index1 = (voxel.center.y - minBounds.y) / (maxBounds.y - minBounds.y) * gridResolutionY;
        index2 = (voxel.center.z - minBounds.z) / (maxBounds.z - minBounds.z) * gridResolutionZ;
    } else if (projectionAxis == 1) { // Rayon le long de Y
        index1 = (voxel.center.x - minBounds.x) / (maxBounds.x - minBounds.x) * gridResolutionX;
        index2 = (voxel.center.z - minBounds.z) / (maxBounds.z - minBounds.z) * gridResolutionZ;
    } else if (projectionAxis == 2) { // Rayon le long de Z
        index1 = (voxel.center.x - minBounds.x) / (maxBounds.x - minBounds.x) * gridResolutionX;
        index2 = (voxel.center.y - minBounds.y) / (maxBounds.y - minBounds.y) * gridResolutionY;
    } else {
        throw std::invalid_argument("Invalid projectionAxis value. Must be 0, 1, or 2.");
    }

    // Combiner les indices secondaires pour obtenir un identifiant unique
    return index1 * gridResolutionZ + index2;
}

int RegularGrid::getVoxelIndex(int x, int y, int z) const {
    // Vérifiez si les coordonnées sont en dehors des limites
    if (x < 0 || x >= gridResolutionX || 
        y < 0 || y >= gridResolutionY || 
        z < 0 || z >= gridResolutionZ) {
        return -1; // Retourne -1 si hors limites
    }

    // Si les coordonnées sont valides, calculer l'indice
    return x * gridResolutionY * gridResolutionZ + y * gridResolutionZ + z;
}

int RegularGrid::getVoxelIndex(const VoxelData&voxel) const {
    glm::vec3 vec3Index = getVoxelVec3Index(voxel);
    return vec3Index.x * gridResolutionY * gridResolutionZ + vec3Index.y * gridResolutionZ + vec3Index.z;
}

glm::vec3 RegularGrid::getVoxelVec3Index(const VoxelData&voxel) const {
    // S'assurer que la position est dans la grille
    glm::vec3 relativePosition = voxel.center - minBounds;

    // Calculer l'indice en divisant par la taille du voxel
    glm::vec3 voxelIndex = glm::floor(relativePosition / (voxel.halfSize * 2));

    // Vérifier si l'indice est dans les limites de la grille
    if (voxelIndex.x < 0 || voxelIndex.x >= gridResolutionX ||
        voxelIndex.y < 0 || voxelIndex.y >= gridResolutionY ||
        voxelIndex.z < 0 || voxelIndex.z >= gridResolutionZ) {
        throw std::out_of_range("Position hors des limites de la grille !");
    }
    return voxelIndex;
}

void RegularGrid::update(float deltaTime, GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        if (!keyYUpPressed) {
            keyYUpPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(voxelVec3Idx.x, std::min(((int)voxelVec3Idx.y + 1), (gridResolutionY - 1)), voxelVec3Idx.z)];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyYUpPressed = false;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!keyYDownPressed) {
            keyYDownPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(voxelVec3Idx.x, std::max(((int)voxelVec3Idx.y - 1), 0), voxelVec3Idx.z)];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyYDownPressed = false;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!keyXUpPressed) {
            keyXUpPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(std::min(((int)voxelVec3Idx.x + 1), (gridResolutionX - 1)), voxelVec3Idx.y, voxelVec3Idx.z)];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyXUpPressed = false;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!keyXDownPressed) {
            keyXDownPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(std::max((int)voxelVec3Idx.x - 1, 0), voxelVec3Idx.y, voxelVec3Idx.z)];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyXDownPressed = false;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (!keyZUpPressed) {
            keyZUpPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(voxelVec3Idx.x, voxelVec3Idx.y, std::min(((int)voxelVec3Idx.z + 1), (gridResolutionZ - 1)))];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyZUpPressed = false;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!keyZDownPressed) {
            keyZDownPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isSelected = 0;
            // std::cout << "Old selected: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
            
            selectedVoxel = &voxels[getVoxelIndex(voxelVec3Idx.x, voxelVec3Idx.y, std::max((int)voxelVec3Idx.z - 1, 0))];
            selectedVoxel->isSelected = 1;
            std::cout << "New selected: "  << getVoxelVec3Index(*selectedVoxel).x << "; " << getVoxelVec3Index(*selectedVoxel).y << "; " << getVoxelVec3Index(*selectedVoxel).z << std::endl; // Forward
        }
    } else
        keyZDownPressed = false;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!keyAddPressed) {
            keyAddPressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isEmpty = 0;
            activeCorner.push_back(selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, -selectedVoxel->halfSize, -selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, -selectedVoxel->halfSize, -selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, -selectedVoxel->halfSize, selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, -selectedVoxel->halfSize, selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, selectedVoxel->halfSize, -selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, selectedVoxel->halfSize, -selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, selectedVoxel->halfSize, selectedVoxel->halfSize));
            activeCorner.push_back(selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, selectedVoxel->halfSize, selectedVoxel->halfSize)); 
 
            std::cout << "Adding voxel at: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
       }
    } else
        keyAddPressed = false;
    if (glfwGetKey(window, GLFW_KEY_SEMICOLON) == GLFW_PRESS) {
        if (!keyDeletePressed) {
            keyDeletePressed = true;
            glm::vec3 voxelVec3Idx = getVoxelVec3Index(*selectedVoxel);
            selectedVoxel->isEmpty = 1;
                        activeCorner.push_back(selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, -selectedVoxel->halfSize, -selectedVoxel->halfSize));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, -selectedVoxel->halfSize, -selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, -selectedVoxel->halfSize, selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, -selectedVoxel->halfSize, selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, selectedVoxel->halfSize, -selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, selectedVoxel->halfSize, -selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(selectedVoxel->halfSize, selectedVoxel->halfSize, selectedVoxel->halfSize)));
            activeCorner.erase(std::find(activeCorner.begin(), activeCorner.end(), selectedVoxel->center + glm::vec3(-selectedVoxel->halfSize, selectedVoxel->halfSize, selectedVoxel->halfSize))); 
 
            std::cout << "Delete voxel at: " << voxelVec3Idx.x << "; " << voxelVec3Idx.y << "; " << voxelVec3Idx.z << std::endl; // Forward
       }
    } else
        keyDeletePressed = false;
    initializeBuffers();
}

bool RegularGrid::intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    
    if (a > -LITTLE_EPSILON && a < LITTLE_EPSILON) return false; // Rayon parallèle au triangle
    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    if (v < 0.0f || u + v > 1.0f) return false;
    t = f * glm::dot(edge2, q);
    return t > LITTLE_EPSILON; // Intersection trouvée
}

void RegularGrid::processRaycastingForAxis(const std::vector<unsigned short>& indices,
                                           const std::vector<glm::vec3>& vertices,
                                           int projectionAxis) {
    // Variables pour les dimensions secondaires
    int primaryResolution, secondaryResolution1, secondaryResolution2;
    if (projectionAxis == 0) { // Rayon le long de X
        primaryResolution = gridResolutionX;
        secondaryResolution1 = gridResolutionY;
        secondaryResolution2 = gridResolutionZ;
    } else if (projectionAxis == 1) { // Rayon le long de Y
        primaryResolution = gridResolutionY;
        secondaryResolution1 = gridResolutionX;
        secondaryResolution2 = gridResolutionZ;
    } else if (projectionAxis == 2) { // Rayon le long de Z
        primaryResolution = gridResolutionZ;
        secondaryResolution1 = gridResolutionX;
        secondaryResolution2 = gridResolutionY;
    } else {
        throw std::invalid_argument("Invalid projectionAxis value. Must be 0, 1, or 2.");
    }
    std::cout << "Resolutions onAxes : " << projectionAxis << " : primaryResolution=" << primaryResolution << ", secondaryResolution1=" << secondaryResolution1 << ", secondaryResolution2=" << secondaryResolution2 << std::endl;

    // Parcourir chaque "colonne" sur les dimensions secondaires
    for (int i = 0; i < secondaryResolution1; ++i) {
        for (int j = 0; j < secondaryResolution2; ++j) {
            // Origine du rayon : premier voxel dans la colonne
            glm::vec3 rayOrigin;
            glm::vec3 rayDir;

            if (projectionAxis == 0) {
                rayOrigin = getVoxel(0, i, j).center - glm::vec3(getVoxel(0, i, j).halfSize + EPSILON, 0.0f, 0.0f);
                rayDir = glm::vec3(1.0f, 0.0f, 0.0f);
            } else if (projectionAxis == 1) {
                rayOrigin = getVoxel(i, 0, j).center - glm::vec3(0.0f, getVoxel(i, 0, j).halfSize + EPSILON, 0.0f);
                rayDir = glm::vec3(0.0f, 1.0f, 0.0f);
            } else if (projectionAxis == 2) {
                rayOrigin = getVoxel(i, j, 0).center - glm::vec3(0.0f, 0.0f, getVoxel(i, j, 0).halfSize + EPSILON);
                rayDir = glm::vec3(0.0f, 0.0f, 1.0f);
            }

            // Liste des intersections
            std::vector<float> intersections;

            // Tester chaque triangle
            for (size_t k = 0; k < indices.size(); k += 3) {
                unsigned short idx0 = indices[k];
                unsigned short idx1 = indices[k + 1];
                unsigned short idx2 = indices[k + 2];
                const glm::vec3& v0 = vertices[idx0];
                const glm::vec3& v1 = vertices[idx1];
                const glm::vec3& v2 = vertices[idx2];

                float t;
                if (intersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, t)) {
                    t += (projectionAxis == 0) ? getVoxel(0, i, j).center.x\
                       : (projectionAxis == 1) ? getVoxel(i, 0, j).center.y\
                       : getVoxel(i, j, 0).center.z;
                    intersections.push_back(t);
                }
            }

            // Trier les intersections
            std::sort(intersections.begin(), intersections.end());

            // Marquer les voxels entre les intersections
            bool isInside = false;
            for (int p = 0; p < primaryResolution; ++p) {
                VoxelData voxel = (projectionAxis == 0) ? getVoxel(p, i, j)\
                                : (projectionAxis == 1) ? getVoxel(i, p, j)\
                                : (projectionAxis == 2) ? getVoxel(i, j, p)\
                                : getVoxel(0, 0, 0);

                float voxelStart = voxel.center[projectionAxis] - voxel.halfSize;
                float voxelEnd = voxel.center[projectionAxis] + voxel.halfSize;

                for (size_t k = 0; k < intersections.size(); ++k) {
                    if (intersections[k] >= voxelStart && intersections[k] <= voxelEnd) {
                        isInside = !isInside;
                    }
                }

                int voxelIndex = (projectionAxis == 0) ? getVoxelIndex(p, i, j)\
                                : (projectionAxis == 1) ? getVoxelIndex(i, p, j)\
                                : getVoxelIndex(i, j, p);
                voxels[voxelIndex].isEmptyOnAxe[projectionAxis] = isInside ? 0 : 1;
            }
        }
    }
}

void RegularGrid::voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices) {
    activeCorner.clear(); 
    if (indices.size() % 3 != 0) {
        std::cerr << "Error: The index data is not valid. Must be a multiple of 3 (triangles)." << std::endl;
        return;
    }
    if (indices.empty() || vertices.empty()) {
        std::cerr << "Error: Mesh data is empty. Ensure you have valid indices and vertices." << std::endl;
        return;
    }

    // Parcourir chaque voxel
    for (VoxelData& voxel : voxels) {
        glm::vec3 rayOrigin = voxel.center - glm::vec3(EPSILON, 0.0f, 0.0f);
        glm::vec3 rayDir(1.0f, 0.0f, 0.0f); // Rayon parallèle à l'axe X
        int intersectionCount = 0;
        // Tester chaque triangle du maillage
        for (size_t i = 0; i < indices.size(); i += 3) {
            // Récupérer les indices des sommets du triangle
            unsigned short idx0 = indices[i];
            unsigned short idx1 = indices[i + 1];
            unsigned short idx2 = indices[i + 2];
            // Récupérer les positions des sommets à partir de la liste `vertices`
            const glm::vec3& v0 = vertices[idx0];
            const glm::vec3& v1 = vertices[idx1];
            const glm::vec3& v2 = vertices[idx2];
            // Tester l'intersection avec le triangle
            float t; // Distance de l'intersection le long du rayon
            if (intersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, t)) {
                intersectionCount++;
            }
        }
        // Utiliser la parité pour déterminer si le voxel est "à l'intérieur"
        if(intersectionCount % 2 == 0){ // Pair -> à l'extérieur
            voxel.isEmpty = 1; 
        }else{
            voxel.isEmpty = 0;
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, voxel.halfSize)); 
        }
    }
    std::cout << "Voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
}

// Méthode de voxelisation de la surface du maillage
void RegularGrid::voxelizeMeshSurface(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices) {
    activeCorner.clear(); 
    if (indices.size() % 3 != 0) {
        std::cerr << "Error: The index data is not valid. Must be a multiple of 3 (triangles)." << std::endl;
        return;
    }
    if (indices.empty() || vertices.empty()) {
        std::cerr << "Error: Mesh data is empty. Ensure you have valid indices and vertices." << std::endl;
        return;
    }

    // Réinitialiser tous les voxels à vide
    for (VoxelData& voxel : voxels) {
        voxel.isEmpty = 1;
    }

    // Parcourir les triangles
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned short idx0 = indices[i];
        unsigned short idx1 = indices[i + 1];
        unsigned short idx2 = indices[i + 2];
        const glm::vec3& v0 = vertices[idx0];
        const glm::vec3& v1 = vertices[idx1];
        const glm::vec3& v2 = vertices[idx2];

        // Déterminer les voxels impactés
        glm::vec3 triMin = glm::min(glm::min(v0, v1), v2);
        glm::vec3 triMax = glm::max(glm::max(v0, v1), v2);
        glm::ivec3 startIdx = glm::floor((triMin - minBounds) / (2 * voxels[0].halfSize));
        glm::ivec3 endIdx = glm::ceil((triMax - minBounds) / (2 * voxels[0].halfSize));

        // Parcourir les voxels dans cette boîte englobante
        for (int x = startIdx.x; x <= endIdx.x; ++x) {
            for (int y = startIdx.y; y <= endIdx.y; ++y) {
                for (int z = startIdx.z; z <= endIdx.z; ++z) {
                    int voxelIndex = getVoxelIndex(x, y, z);
                    if (voxelIndex < 0 || voxelIndex >= voxels.size()) continue;

                    VoxelData& voxel = voxels[voxelIndex];
                    glm::vec3 boxCenter = voxel.center;
                    glm::vec3 boxHalfSize(voxel.halfSize + EPSILON);

                    if (Grid::triangleIntersectsAABB(v0, v1, v2, boxCenter, boxHalfSize)) {
                        voxel.isEmpty = 0; // Marquer le voxel comme "touché"
                        activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, -voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, -voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, voxel.halfSize));
                        activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, voxel.halfSize)); 
            
                    }
                }
            }
        }
    }

    std::cout << "Surface voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
}

void RegularGrid::optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, 
                                        const std::vector<glm::vec3>& vertices) {
    activeCorner.clear(); 
    if (indices.size() % 3 != 0) {
        std::cerr << "Error: The index data is not valid. Must be a multiple of 3 (triangles)." << std::endl;
        return;
    }
    if (indices.empty() || vertices.empty()) {
        std::cerr << "Error: Mesh data is empty. Ensure you have valid indices and vertices." << std::endl;
        return;
    }

    // Marquer tous les voxels comme vides
    for (VoxelData& voxel : voxels) {
        voxel.isEmpty = 1;
        voxel.isEmptyOnAxe = glm::vec3(1, 1, 1);
    }

    // Lancer des rayons sur chaque axe
    processRaycastingForAxis(indices, vertices, 0); // Axe X
    processRaycastingForAxis(indices, vertices, 1); // Axe Y
    processRaycastingForAxis(indices, vertices, 2); // Axe Z

    for (VoxelData &voxel : voxels) {
        voxel.isEmpty = voxel.isEmptyOnAxe.x == 1 || voxel.isEmptyOnAxe.y == 1 || voxel.isEmptyOnAxe.z == 1;
        // On marque tous les bords du voxel comme actifs
        if (!voxel.isEmpty){
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, -voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, voxel.halfSize));
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, voxel.halfSize)); 
            
        }
    }

    std::cout << "Optimized voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
}


void RegularGrid::printGrid() const {
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
    for (int y = 0; y < resolutionY; ++y) {
        for (int z = 0; z < resolutionZ; ++z) {
            for (int x = 0; x < resolutionX; ++x) {
                // Trouver le voxel correspondant
                int index = y * resolutionZ * resolutionX + z * resolutionX + x;
                const VoxelData& voxel = voxels[index];
                // Afficher 1 si le voxel est rempli, sinon afficher 0
                std::cout << voxel.isEmpty << " ";
            }
            std::cout << std::endl; // Nouvelle ligne après chaque ligne de voxels
        }
        std::cout << std::endl; // Séparer les couches de voxels
    }
}
void RegularGrid::marchingCube( std::vector<unsigned short> &indices, std::vector<glm::vec3> &vertices) {

    removeDuplicates(activeCorner);

    glm::vec3 gridSize = maxBounds - minBounds;
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});
    float halfSize = voxelSize / 2;

    // Parcours du volume de voxels
    for (int x = 0; x < gridResolutionX + 2; ++x) {
        for (int y = 0; y < gridResolutionY + 2; ++y) {
            for (int z = 0; z < gridResolutionZ + 2; ++z) {
                // Décalage des positions des voxels pour englober tous les voxels
                glm::vec3 center = minBounds + glm::vec3(x - 1, y - 1, z - 1) * voxelSize + glm::vec3(halfSize);

                glm::vec3 corners[8] = {
                    center + glm::vec3(-halfSize, -halfSize, -halfSize),
                    center + glm::vec3(halfSize, -halfSize, -halfSize),
                    center + glm::vec3(halfSize, -halfSize, halfSize),
                    center + glm::vec3(-halfSize, -halfSize, halfSize),
                    center + glm::vec3(-halfSize, halfSize, -halfSize),
                    center + glm::vec3(halfSize, halfSize, -halfSize),
                    center + glm::vec3(halfSize, halfSize, halfSize),
                    center + glm::vec3(-halfSize, halfSize, halfSize)
                };

                // Calcul du cube index pour identifier les coins "pleins"
                int cubeIndex = 0;
                for (int j = 0; j < 8; j++) {
                    for (glm::vec3 key : activeCorner) {
                        if (glm::distance(key, corners[j]) < 0.001f) {
                            cubeIndex |= (1 << j);
                            break;
                        }
                    }
                }

                // Récupérer la table de triangulation pour ce cube
                const int* triangulationData = MarchingCubesTable::triangulation[cubeIndex];

                // Traiter les triangles du cube
                for (int k = 0; k < 16; k += 3) {
                    if (triangulationData[k] == -1 || triangulationData[k + 1] == -1 || triangulationData[k + 2] == -1) {
                        break; // Fin des triangles pour ce cube
                    }

                    // Récupérer les indices des coins à interpoler pour les 3 arêtes d'un triangle
                    int a = MarchingCubesTable::cornerIndexAFromEdge[triangulationData[k]];
                    int b = MarchingCubesTable::cornerIndexBFromEdge[triangulationData[k]];

                    int a1 = MarchingCubesTable::cornerIndexAFromEdge[triangulationData[k + 1]];
                    int b1 = MarchingCubesTable::cornerIndexBFromEdge[triangulationData[k + 1]];

                    int a2 = MarchingCubesTable::cornerIndexAFromEdge[triangulationData[k + 2]];
                    int b2 = MarchingCubesTable::cornerIndexBFromEdge[triangulationData[k + 2]];
                    // addTriangle((corners[a] + corners[b]) * 0.5f, (corners[a1] + corners[b1]) * 0.5f, (corners[a2] + corners[b2]) * 0.5f, vertices, indices); 

                    vertices.push_back((corners[a] + corners[b])*0.5f); 
                    vertices.push_back((corners[a1] + corners[b1])*0.5f); 
                    vertices.push_back((corners[a2] + corners[b2])*0.5f); 

                    indices.push_back(vertices.size()-3);
                    indices.push_back(vertices.size()-2);
                    indices.push_back(vertices.size()-1);
                   
                }
            }
        }
    }
}


