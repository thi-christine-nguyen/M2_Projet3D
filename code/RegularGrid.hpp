#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "variables.hpp"
#include "Voxel.hpp"  // Inclusion de la nouvelle classe Voxel

class RegularGrid {
private:
    glm::vec3 min; // Coordonnées minimales
    glm::vec3 max; // Coordonnées maximalesGLuint vao = 0;  // Vertex Array Object

    std::vector<Voxel> voxels;       // Liste des voxels

    bool initialized = false;
    int resolution;  // Résolution initiale (base)

public:
    // --- Constructeurs ---
    RegularGrid() {}
    RegularGrid(glm::vec3 min, glm::vec3 max, int resolution)
        : min(min), max(max), resolution(resolution) {}

    // --- Accesseurs ---
    glm::vec3 getMin() const { return min; }
    glm::vec3 getMax() const { return max; }
    int getResolution() const { return resolution; }
    const std::vector<Voxel>& getVoxels() const { return voxels; }

    // --- Initialisation ---
    // Initialiser avec des sommets
    void init(const std::vector<glm::vec3>& _vertices, int _resolution = 10) {
        if (_vertices.empty()) {
            std::cout << "Liste de vertices vide, grille non initialisée." << std::endl;
            return;
        }
        resolution = _resolution;
        glm::vec3 minVertex = _vertices[0];
        glm::vec3 maxVertex = _vertices[0];

        for (const auto& vertex : _vertices) {
            minVertex = glm::min(minVertex, vertex);
            maxVertex = glm::max(maxVertex, vertex);
        }

        min = minVertex;
        max = maxVertex;

        // Régénérer les voxels
        generateVoxels();
        // generateVoxels();
        initialized = true;
    }

    void generateVoxels() {
        // Dimensions de la grille
        glm::vec3 gridSize = max - min;

        // Calculer la taille réelle d'un voxel cubique
        float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

        // Calculer les nouvelles résolutions pour couvrir complètement la grille
        int resolutionX = std::ceil(gridSize.x / voxelSize);
        int resolutionY = std::ceil(gridSize.y / voxelSize);
        int resolutionZ = std::ceil(gridSize.z / voxelSize);

        std::cout << "Voxel size (cubique): " << voxelSize << std::endl;
        std::cout << "Resolutions adjusted: X=" << resolutionX << ", Y=" << resolutionY << ", Z=" << resolutionZ << std::endl;

        voxels.clear();

        // Générer les voxels
        for (int y = 0; y < resolutionY; y++) {
            for (int x = 0; x < resolutionX; x++) {
                for (int z = 0; z < resolutionZ; z++) {
                    glm::vec3 minCorner = min + glm::vec3(x, y, z) * voxelSize;
                    glm::vec3 center = minCorner + glm::vec3(voxelSize * 0.5f);

                    // Créer un voxel cubique et l'ajouter à la liste
                    voxels.emplace_back(center, voxelSize);
                    // std::cout << center.x << "; " << center.y << "; " << center.z << std::endl;

                    // Init les buffers du voxel
                    // voxels.back().initializeBuffers();
                }
            }
        }
        std::cout << "Generated " << voxels.size() << " cubic voxels." << std::endl;
    }

    // Mettre à jour la bounding box après une transformation
    void updateAfterTransformation(const std::vector<glm::vec3>& vertices, const glm::mat4& transform) {
        std::vector<glm::vec3> transformedVertices;

        for (const auto& vertex : vertices) {
            glm::vec4 transformedVertex = transform * glm::vec4(vertex, 1.0f);
            transformedVertices.push_back(glm::vec3(transformedVertex));
        }

        init(transformedVertices);
    }

    bool intersectRayTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, 
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

    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices) {
        if (indices.size() % 3 != 0) {
            std::cerr << "Error: The index data is not valid. Must be a multiple of 3 (triangles)." << std::endl;
            return;
        }

        if (indices.empty() || vertices.empty()) {
            std::cerr << "Error: Mesh data is empty. Ensure you have valid indices and vertices." << std::endl;
            return;
        }

        // Parcourir chaque voxel
        for (auto& voxel : voxels) {
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
            voxel.isEmpty = (intersectionCount % 2 == 0); // Pair -> à l'extérieur
        }

        std::cout << "Voxelization complete: " << voxels.size() << " voxels processed." << std::endl;
    }

    void printGrid() const {
        // Dimensions de la grille
        glm::vec3 gridSize = getMax() - getMin();

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
                    const Voxel& voxel = voxels[index];

                    // Afficher 1 si le voxel est rempli, sinon afficher 0
                    std::cout << (voxel.isEmpty ? '0' : '1') << " ";
                }
                std::cout << std::endl; // Nouvelle ligne après chaque ligne de voxels
            }
            std::cout << std::endl; // Séparer les couches de voxels
        }
    }

    // --- Dessin ---
    void draw() const {
        if (!initialized) {
            std::cerr << "RegularGrid not initialized! Call initBuffers() first." << std::endl;
            return;
        }

        for (auto& voxel : voxels) {
            voxel.draw();
        }

        // glBindVertexArray(0);
    }

    // --- Destructeur ---
    ~RegularGrid() {}
};

#endif
