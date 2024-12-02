#ifndef VOXEL_HPP__
#define VOXEL_HPP__

#include <glm/glm.hpp>

class Voxel {
public:
    glm::vec3 center;  // Centre du voxel
    float size;        // Taille uniforme (côté du cube)
    bool isEmpty;      // Indique si le voxel est vide

    // --- Constructeurs ---
    Voxel() : center(0.0f), size(1.0f), isEmpty(true) {}
    Voxel(const glm::vec3& center, float size, bool isEmpty = true)
        : center(center), size(size), isEmpty(isEmpty) {}

    // --- Destructeur ---
    ~Voxel() = default;
};

#endif