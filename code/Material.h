#include <cmath>

#include <GL/glut.h>

class Material {
public:  // Modifier la section d'accès à public pour le constructeur
    glm::vec3 ambient_material;
    glm::vec3 diffuse_material;
    glm::vec3 specular_material;
    double shininess;

    // Constructeur
    Material(glm::vec3 ambient =  glm::vec3(0.19225, 0.19225, 0.19225),
             glm::vec3 diffuse = glm::vec3(0.50754, 0.50754, 0.50754),
             glm::vec3 specular = glm::vec3(0.508273, 0.508273, 0.508273),
             double shininess = 51.0)
        : ambient_material(ambient), diffuse_material(diffuse), specular_material(specular), shininess(shininess) {}
};