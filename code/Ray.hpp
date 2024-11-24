#include <glm/glm.hpp>
#include <iostream>

#define EPSILON 1e-6

// Définir la classe Ray
class Ray {
public:
    glm::vec3 origin;  // Origine du rayon
    glm::vec3 direction; // Direction du rayon

    // Constructeur
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin(origin), direction(glm::normalize(direction)) {}

    // Fonction pour tester l'intersection avec un triangle
    bool intersectsTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) const {
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 h = glm::cross(direction, edge2);
        float a = glm::dot(edge1, h);

        // Si a est très proche de zéro, le rayon est parallèle au triangle
        if (a > -EPSILON && a < EPSILON) {
            return false;
        }

        float f = 1.0f / a;
        glm::vec3 s = origin - v0;
        float u = f * glm::dot(s, h);

        // Si u est hors du triangle, pas d'intersection
        if (u < 0.0f || u > 1.0f) {
            return false;
        }

        glm::vec3 q = glm::cross(s, edge1);
        float v = f * glm::dot(direction, q);

        // Si v est hors du triangle, pas d'intersection
        if (v < 0.0f || u + v > 1.0f) {
            return false;
        }

        t = f * glm::dot(edge2, q); // Calculer la distance du rayon à l'intersection

        // Si t est supérieur à l'epsilon, il y a une intersection dans le sens du rayon
        return t > EPSILON;
    }

    // Optionnel : vous pouvez également ajouter des méthodes pour interagir avec le rayon
    // par exemple, pour obtenir un point d'intersection à une distance t donnée.
    glm::vec3 getPointAt(float t) const {
        return origin + direction * t;
    }
};

