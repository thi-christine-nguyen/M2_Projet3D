#include <cmath>

#include <GL/glut.h>

class Material {
public:  // Modifier la section d'accès à public pour le constructeur
    glm::vec3 ambient_material;
    glm::vec3 diffuse_material;
    glm::vec3 specular_material;
    double shininess;

    // Constructeur
    Material(glm::vec3 ambient = glm::vec3(0., 0., 0.),
             glm::vec3 diffuse = glm::vec3(1., 1., 1.),
             glm::vec3 specular = glm::vec3(1., 1., 1.),
             double shininess = 16.0)
        : ambient_material(ambient), diffuse_material(diffuse), specular_material(specular), shininess(shininess) {}
};