#ifndef ADAPTATIVE_GRID_HPP__
#define ADAPTATIVE_GRID_HPP__

#include <memory>
#include "Grid.hpp"

struct OctreeNode {
    glm::vec3 minBounds, maxBounds;     // Limites du nœud
    bool isLeaf;                        // Si le nœud est une feuille
    std::vector<OctreeNode> children;   // Enfants du nœud (8 max)

    OctreeNode(const glm::vec3& minBounds, const glm::vec3& maxBounds)
        : minBounds(minBounds - EPSILON), maxBounds(maxBounds + EPSILON), isLeaf(true) {}

    void subdivide() {
        glm::vec3 size = (maxBounds - minBounds) * 0.5f;
        glm::vec3 center = (minBounds + maxBounds) * 0.5f;

        children.clear();
        for (int i = 0; i < 8; ++i) {
            glm::vec3 offset = glm::vec3((i & 1) ? size.x : 0, 
                                         (i & 2) ? size.y : 0, 
                                         (i & 4) ? size.z : 0);
            children.emplace_back(minBounds + offset, center + offset);
        }
        isLeaf = false;
    }

    void print(int depth = 0) const {
        // Affiche l'indentation en fonction de la profondeur
        std::string indent(depth * 2, ' ');

        // Affiche les informations du nœud
        std::cout << indent << "Node [" 
                << "Min: (" << minBounds.x << ", " << minBounds.y << ", " << minBounds.z << "), "
                << "Max: (" << maxBounds.x << ", " << maxBounds.y << ", " << maxBounds.z << "), "
                << "Is Leaf: " << (isLeaf ? "Yes" : "No") << ", "
                << "Children: " << children.size() << "]" << std::endl;

        // Appelle récursivement print sur les enfants
        for (const auto& child : children) {
            child.print(depth + 1);
        }
    }

    // Méthode utilitaire pour tester un axe de séparation
    bool testAxis(const glm::vec3& axis, const glm::vec3& t0, const glm::vec3& t1, const glm::vec3& t2,
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

    bool intersectsTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) const {
        glm::vec3 boxCenter = (minBounds + maxBounds) * 0.5f;
        glm::vec3 boxHalfSize = (maxBounds - minBounds) * 0.5f;

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
    
};

class AdaptativeGrid : public Grid {
private:
    std::shared_ptr<OctreeNode> root;
    

public:
    AdaptativeGrid() {};
    AdaptativeGrid(const glm::vec3& minBounds, const glm::vec3& maxBounds, int resolution, VoxelizationMethod method);
    AdaptativeGrid(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices, int resolution, VoxelizationMethod method);

    void printGrid() const;

    void voxelizeMesh(const std::vector<unsigned short>& indices, const std::vector<glm::vec3>& vertices);
    void voxelizeNode(OctreeNode& node, const std::vector<unsigned short>& indices,
                    const std::vector<glm::vec3>& vertices, int depth);
    void fillVoxelDataRecursive(const OctreeNode& node);
    void marchingCube( std::vector<unsigned short> &indices, std::vector<glm::vec3> &vertices) override;
    void marchOctreeNode(OctreeNode* node, std::vector<unsigned short>& indices, std::vector<glm::vec3>& vertices);

    virtual ~AdaptativeGrid() = default;
};

#endif
