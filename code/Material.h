#include <cmath>
#include <GL/glut.h>

class Material {
private:
    // Membres de données
    glm::vec3 ambient_material;
    glm::vec3 diffuse_material;
    glm::vec3 specular_material;
    double shininess;
public:
    // Constructeur
    Material(const glm::vec3& ambient = glm::vec3(0.2, 0.2, 0.2),
             const glm::vec3& diffuse = glm::vec3(0.5, 0.5, 0.5),
             const glm::vec3& specular = glm::vec3(0.5, 0.5, 0.5),
             double shininess = 32.0)
        : ambient_material(ambient), diffuse_material(diffuse), specular_material(specular), shininess(shininess) {}

    // Getters
    const glm::vec3& getAmbient() const { return ambient_material; }
    const glm::vec3& getDiffuse() const { return diffuse_material; }
    const glm::vec3& getSpecular() const { return specular_material; }
    double getShininess() const { return shininess; }

    // Setters
    void setAmbient(const glm::vec3& ambient) { ambient_material = ambient; }
    void setDiffuse(const glm::vec3& diffuse) { diffuse_material = diffuse; }
    void setSpecular(const glm::vec3& specular) { specular_material = specular; }
    void setShininess(double shininess) { this->shininess = shininess; }

};