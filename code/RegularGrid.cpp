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
    Grid::initializeBuffers();
}

void RegularGrid::generateVoxels() {
     // Dimensions de la grille
    glm::vec3 gridSize = maxBounds - minBounds;

    // Calculer la taille réelle d'un voxel cubique
    float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

    // Calculer les nouvelles résolutions pour couvrir complètement la grille
    gridResolutionX = std::ceil(gridSize.x / voxelSize);
    gridResolutionY = std::ceil(gridSize.y / voxelSize);
    gridResolutionZ = std::ceil(gridSize.z / voxelSize);
    float halfSize = voxelSize / 2;
    std::cout << "Voxel size (cubique): " << voxelSize << ", halfSize: " << halfSize << std::endl;
    std::cout << "Resolutions adjusted: X=" << gridResolutionX << ", Y=" << gridResolutionY << ", Z=" << gridResolutionZ << std::endl;

    voxels.clear();

    for (int x = 0; x < gridResolutionX; ++x) {
        for (int y = 0; y < gridResolutionY; ++y) {
            for (int z = 0; z < gridResolutionZ; ++z) {
                glm::vec3 center = minBounds + glm::vec3(x, y, z) * voxelSize + glm::vec3(halfSize);
                voxels.emplace_back(center, halfSize, 1);
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
    return x * gridResolutionY * gridResolutionZ + y * gridResolutionZ + z;
}


bool RegularGrid::intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
                        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) {
    const float EPSILON = 1e-6f;
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 h = glm::cross(rayDir, edge2);
    float a = glm::dot(edge1, h);
    
    if (a > -EPSILON && a < EPSILON) return false; // Rayon parallèle au triangle
    float f = 1.0f / a;
    glm::vec3 s = rayOrigin - v0;
    float u = f * glm::dot(s, h);
    if (u < 0.0f || u > 1.0f) return false;
    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(rayDir, q);
    if (v < 0.0f || u + v > 1.0f) return false;
    t = f * glm::dot(edge2, q);
    return t > EPSILON; // Intersection trouvée
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
            const float EPSILON = 1e-6f;

            if (projectionAxis == 0) {
                rayOrigin = getVoxel(0, i, j).center - glm::vec3(getVoxel(0, i, j).halfSize, 0.0f, 0.0f) -EPSILON;
                rayDir = glm::vec3(1.0f, 0.0f, 0.0f);
            } else if (projectionAxis == 1) {
                rayOrigin = getVoxel(i, 0, j).center - glm::vec3(0.0f, getVoxel(0, i, j).halfSize, 0.0f) -EPSILON;
                rayDir = glm::vec3(0.0f, 1.0f, 0.0f);
            } else if (projectionAxis == 2) {
                rayOrigin = getVoxel(i, j, 0).center - glm::vec3(0.0f, 0.0f, getVoxel(0, i, j).halfSize) -EPSILON;
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
        glm::vec3 rayOrigin = voxel.center;
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
        voxel.isEmpty = (intersectionCount % 2 == 0) ? 1 : 0; // Pair -> à l'extérieur
    }
    std::cout << "Voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
}

// Méthode de voxelisation de la surface du maillage
void RegularGrid::voxelizeMeshSurface(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices) {
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
                    glm::vec3 boxHalfSize(voxel.halfSize);

                    if (Grid::triangleIntersectsAABB(v0, v1, v2, boxCenter, boxHalfSize)) {
                        voxel.isEmpty = 0; // Marquer le voxel comme "touché"
                    }
                }
            }
        }
    }

    std::cout << "Surface voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
}

void RegularGrid::optimizedVoxelizeMesh(const std::vector<unsigned short>& indices, 
                                        const std::vector<glm::vec3>& vertices) {
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
        // if (voxel.isEmpty)
        //     std::cout << voxel.isEmpty << ": " << voxel.isEmptyOnAxe.x << "; " << voxel.isEmptyOnAxe.y << "; " << voxel.isEmptyOnAxe.z << std::endl;
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


void RegularGrid::marchingCube() {
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    glm::vec3 corner[8]; 
    std::vector<glm::vec3> activeCorner; 
    for (VoxelData &voxel : voxels) {
        if(!voxel.isEmpty){

            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, -voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, -voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, -voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, -voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, voxel.halfSize)); 
            activeCorner.push_back(voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, voxel.halfSize)); 
            
            voxel.edge[0] = 1;  
            voxel.edge[1] = 1;  
            voxel.edge[2] = 1;  
            voxel.edge[3] = 1;  
            voxel.edge[4] = 1;  
            voxel.edge[5] = 1;  
            voxel.edge[6] = 1;              
            voxel.edge[7] = 1;  
        }
    }

    for (VoxelData &voxel : voxels) {
        if(voxel.isEmpty){
            corner[0] = voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, -voxel.halfSize); 
            corner[1] = voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, -voxel.halfSize); 
            corner[2] = voxel.center + glm::vec3(voxel.halfSize, -voxel.halfSize, voxel.halfSize); 
            corner[3] = voxel.center + glm::vec3(-voxel.halfSize, -voxel.halfSize, voxel.halfSize); 
            corner[4] = voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, -voxel.halfSize); 
            corner[5] = voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, -voxel.halfSize); 
            corner[6] = voxel.center + glm::vec3(voxel.halfSize, voxel.halfSize, voxel.halfSize); 
            corner[7] = voxel.center + glm::vec3(-voxel.halfSize, voxel.halfSize, voxel.halfSize); 

            for(int i = 0; i < 8; i++){
                if (std::find(activeCorner.begin(), activeCorner.end(), corner[i]) != activeCorner.end()) {
                    voxel.edge[i] = 1;
                } else {
                    voxel.edge[i] = 0;
                }
            }
        }
    }

    // for (VoxelData &voxel : voxels) {
    //     int cubeIndex = 0;
    //     for (int i = 0; i < 8; i++) {
    //         if (voxel.edge[i] == 1) {
    //             cubeIndex |= (1 << i);  // On met à 1 le bit correspondant à ce bord
    //         }else{
    //             cubeIndex |= (0 << i);
    //         }
    //     }

    //     // // Si aucune intersection (cubeIndex == 0), continuer
    //     // if (cubeIndex == 0) continue;

    //     // Utiliser la table de triangulation pour obtenir les indices des triangles
    //     const int *triangulationData = MarchingCubeTable::triangulation[cubeIndex];

    //     for (int i = 0; triangulationData[i] != -1; i += 3) {
    //         glm::vec3 vertexA = corner[MarchingCubeTable::cornerIndexAFromEdge[triangulationData[i]]];
    //         glm::vec3 vertexB = corner[MarchingCubeTable::cornerIndexBFromEdge[triangulationData[i + 1]]];
    //         glm::vec3 vertexC = corner[MarchingCubeTable::cornerIndexAFromEdge[triangulationData[i + 2]]];

    //         // Calculer la position des vertices (intersection des bords)
    //         // Exemple basique : intersection lineaire sur chaque bord, à ajuster selon ton critère
    //         glm::vec3 intersectionAB = (vertexA + vertexB) / 2.0f; 
    //         glm::vec3 intersectionBC = (vertexB + vertexC) / 2.0f;
    //         glm::vec3 intersectionCA = (vertexC + vertexA) / 2.0f;

    //         // Ajouter ces vertices à la liste
    //         vertices.push_back(intersectionAB);
    //         vertices.push_back(intersectionBC);
    //         vertices.push_back(intersectionCA);

    //         // Ajouter les indices pour former un triangle
    //         indices.push_back(vertices.size() - 3);
    //         indices.push_back(vertices.size() - 2);
    //         indices.push_back(vertices.size() - 1);
    //     }
    // }
    

    std::ofstream outFile("../data/meshes/output.off");
    if (!outFile) {
        std::cerr << "Erreur : impossible d'ouvrir le fichier pour écrire les données OFF." << std::endl;
        return;
    }

    // Écriture de l'en-tête OFF
    outFile << "OFF\n";
    outFile << vertices.size() << " " << (indices.size() / 3) << " 0\n"; // Nb de sommets, faces, arêtes

    // Écriture des sommets
    for (const auto &vertex : vertices) {
        outFile << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    // Écriture des faces
    for (size_t i = 0; i < indices.size(); i += 3) {
        outFile << "3 " << indices[i] << " " << indices[i + 1] << " " << indices[i + 2] << "\n";
    }

    outFile.close();
    std::cout << "Fichier OFF généré avec succès : output.off" << std::endl;


}

