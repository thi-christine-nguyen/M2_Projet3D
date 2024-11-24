#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include "variables.hpp"
#include "BoundingBox.hpp"
#include "Voxel.hpp"  // Inclusion de la nouvelle classe Voxel

class RegularGrid : public BoundingBox {
private:
    GLuint vao = 0;  // Vertex Array Object
    GLuint vbo = 0;  // Vertex Buffer Object
    GLuint colorULoc;

    std::vector<glm::vec3> vertices; // Sommets pour le rendu des voxels (wireframe)
    std::vector<Voxel> voxels;       // Liste des voxels

    bool initialized = false;
    int resolution;  // Résolution initiale (base)

public:
    // --- Constructeurs ---
    RegularGrid() {}
    RegularGrid(glm::vec3 min, glm::vec3 max, int resolution)
        : BoundingBox(min, max), resolution(resolution) {}

    // --- Initialisation ---
    void initBuffers() {
        if (initialized) return;

        generateVoxels();

        // Création des VAO et VBO
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);

        // Charger les sommets dans le VBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Configurer les attributs de vertex
        glEnableVertexAttribArray(0); // Layout 0 : position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

        glBindVertexArray(0); // Delink le VAO

        colorULoc = glGetUniformLocation(programID, "color");
        initialized = true;
    }

    void generateVoxels() {
        // Dimensions de la grille
        glm::vec3 gridSize = getMax() - getMin();

        // Taille approximative d'un voxel cubique
        float initialVoxelSize = gridSize.x / resolution;

        // Calculer la taille réelle d'un voxel cubique
        float voxelSize = std::min({gridSize.x / resolution, gridSize.y / resolution, gridSize.z / resolution});

        // Calculer les nouvelles résolutions pour couvrir complètement la grille
        int resolutionX = std::ceil(gridSize.x / voxelSize);
        int resolutionY = std::ceil(gridSize.y / voxelSize);
        int resolutionZ = std::ceil(gridSize.z / voxelSize);

        std::cout << "Voxel size (cubique): " << voxelSize << std::endl;
        std::cout << "Resolutions adjusted: X=" << resolutionX << ", Y=" << resolutionY << ", Z=" << resolutionZ << std::endl;

        voxels.clear();
        vertices.clear();

        // Générer les voxels
        for (int y = 0; y < resolutionY; y++) {
            for (int x = 0; x < resolutionX; x++) {
                for (int z = 0; z < resolutionZ; z++) {
                    glm::vec3 minCorner = getMin() + glm::vec3(x, y, z) * voxelSize;
                    glm::vec3 center = minCorner + glm::vec3(voxelSize * 0.5f);

                    // Créer un voxel cubique et l'ajouter à la liste
                    voxels.emplace_back(center, voxelSize);

                    // Ajouter les sommets pour le rendu
                    voxels.back().getVertices(vertices);
                    voxels.back().initializeBuffers();
                }
            }
        }
        std::cout << "Generated " << voxels.size() << " cubic voxels." << std::endl;
    }

    void testVoxelContainment(const std::function<bool(const Voxel&)>& isInsideMesh) {
        // Tester si chaque voxel contient une partie du mesh
        for (auto& voxel : voxels) {
            voxel.isEmpty = !isInsideMesh(voxel);
        }
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

        // glBindVertexArray(vao);

        // // Dessiner les voxels en wireframe
        // glm::vec4 color {0.f, 1.f, 0.f, 1.f};  // Vert par défaut
        // glUniform4fv(colorULoc, 1, &color[0]);
        // glLineWidth(1.f);

        // glDrawArrays(GL_LINES, 0, vertices.size());
        for (auto& voxel : voxels) {
            voxel.render();
        }

        // glBindVertexArray(0);
    }

    // --- Destructeur ---
    ~RegularGrid() {
        if (initialized) {
            glDeleteBuffers(1, &vbo);
            glDeleteVertexArrays(1, &vao);
        }
    }
};

#endif
