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

    // Fonction pour mettre à jour les relations réciproques
    void updateNeighborEdges(int offsetIndex, VoxelData& current, VoxelData& neighbor) {
        // Mapping inverse des voisins
        static const int inverseOffset[] = {
            1, 0, 3, 2, 5, 4, 9, 8, 11, 10, 13, 12, 15, 14, 17, 16,
            19, 18, 21, 20, 23, 22, 25, 24, 27, 26
        };

        int sharedCorners[6][4] = {
            {1, 5, 6, 2}, // +X
            {0, 4, 7, 3}, // -X
            {4, 5, 6, 7}, // +Y
            {0, 1, 2, 3}, // -Y
            {2, 3, 7, 6}, // +Z
            {0, 1, 5, 4}, // -Z
        };

        int sharedArete[12][2] = {
            {5, 6}, {1, 2}, {4, 7}, {0, 3},
            {2, 6}, {1, 5}, {3, 7}, {0, 4},
            {6, 7}, {4, 5}, {2, 3}, {0, 1},
        };

        int sharedEdge[8][1] = {
            {6}, {5}, {2}, {1},
            {7}, {4}, {3}, {0}
        };

        int inverseIdx = inverseOffset[offsetIndex];

        // Répéter la mise à jour en sens inverse
        if (inverseIdx < 6) {
            for (int k = 0; k < 4; k++) {
                int idx = sharedCorners[inverseIdx][k];
                neighbor.edge[idx] = 1;
            }
        } else if (inverseIdx >= 6 && inverseIdx < 18) {
            for (int k = 0; k < 2; k++) {
                int idx = sharedArete[inverseIdx - 6][k];
                neighbor.edge[idx] = 1;
            }
        } else {
            int idx = sharedEdge[inverseIdx - 18][0];
            neighbor.edge[idx] = 1;
        }
    }

    void getVoisin(VoxelData& voxel) {
        glm::vec3 offsets[] = {
            // Voisins directs sur les axes principaux (6)
            glm::vec3(1, 0, 0), glm::vec3(-1, 0, 0),
            glm::vec3(0, 1, 0), glm::vec3(0, -1, 0),
            glm::vec3(0, 0, 1), glm::vec3(0, 0, -1),

            // Voisins diagonaux sur les faces (12)
            glm::vec3(1, 1, 0), glm::vec3(1, -1, 0),
            glm::vec3(-1, 1, 0), glm::vec3(-1, -1, 0),
            glm::vec3(1, 0, 1), glm::vec3(1, 0, -1),
            glm::vec3(-1, 0, 1), glm::vec3(-1, 0, -1),
            glm::vec3(0, 1, 1), glm::vec3(0, 1, -1),
            glm::vec3(0, -1, 1), glm::vec3(0, -1, -1),

            // Voisins sur les coins (8)
            glm::vec3(1, 1, 1), glm::vec3(1, 1, -1),
            glm::vec3(1, -1, 1), glm::vec3(1, -1, -1),
            glm::vec3(-1, 1, 1), glm::vec3(-1, 1, -1),
            glm::vec3(-1, -1, 1), glm::vec3(-1, -1, -1)
        };

        int sharedCorners[6][4] = {
            {1, 5, 6, 2}, // +X
            {0, 4, 7, 3}, // -X
            {4, 5, 6, 7}, // +Y
            {0, 1, 2, 3}, // -Y
            {2, 3, 7, 6}, // +Z
            {0, 1, 5, 4}, // -Z
        };

        int sharedArete[12][2] = {
            {5, 6}, {1, 2}, {4, 7}, {0, 3},
            {2, 6}, {1, 5}, {3, 7}, {0, 4},
            {6, 7}, {4, 5}, {2, 3}, {0, 1},
        };

        int sharedEdge[8][1] = {
            {6}, {5}, {2}, {1},
            {7}, {4}, {3}, {0}
        };

        float epsilon = 0.001f; // Tolérance pour la comparaison des flottants

        for (int j = 0; j < 26; ++j) {
            glm::vec3 neighborCenter = voxel.center + offsets[j] * (2 * voxel.halfSize);

            // Récupérer l'indice du voisin
            int voxelIndx = getVoxelIndex(neighborCenter.x, neighborCenter.y, neighborCenter.z);
            if(voxelIndx == -1){continue;}

            // Vérifier si le voisin existe et s'il n'est pas vide
            if (voxelIndx != -1 && voxels[voxelIndx].isEmpty == 0) {
                // Mise à jour des bords partagés pour le voxel actuel
                if (j < 6) {
                    for (int k = 0; k < 4; k++) {
                        int idx = sharedCorners[j][k];
                        // voxels[voxelIndx].edge[idx] = 1;
                        voxel.edge[idx] = 1; // Mise à jour du voxel courant
                    }
                } else if (j >= 6 && j < 18) {
                    for (int k = 0; k < 2; k++) {
                        int idx = sharedArete[j - 6][k];
                        // voxels[voxelIndx].edge[idx] = 1;
                        voxel.edge[idx] = 1; // Mise à jour du voxel courant
                    }
                } else {
                    int idx = sharedEdge[j - 18][0];
                    // voxels[voxelIndx].edge[idx] = 1;
                    voxel.edge[idx] = 1; // Mise à jour du voxel courant
                }

                // Mise à jour réciproque des voisins
                updateNeighborEdges(j, voxel, voxels[voxelIndx]);
            }
        }
}

    virtual ~RegularGrid() = default;
};

#endif