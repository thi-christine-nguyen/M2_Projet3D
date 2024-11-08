#ifndef TRANSFORM_HPP__
#define TRANSFORM_HPP__

#include "lib.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class Transform {
private:
    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;

public:
    // Constructeur
    Transform(const glm::vec3& pos = glm::vec3(0.0f),
              const glm::vec3& rot = glm::vec3(0.0f),
              const glm::vec3& scl = glm::vec3(1.0f))
        : position_(pos), rotation_(rot), scale_(scl) {}

    // Accesseurs (getters) et mutateurs (setters) pour la position, la rotation et l'échelle
    const glm::vec3& getPosition() const { return position_; }
    void setPosition(const glm::vec3& pos) { position_ = pos; }

    const glm::vec3& getRotation() const { return rotation_; }
    void setRotation(const glm::vec3& rot) { rotation_ = rot; }

    const glm::vec3& getScale() const { return scale_; }
    void setScale(const glm::vec3& scl) { scale_ = scl; }

    // Fonction pour récupérer la matrice de transformation
    glm::mat4 getMatrix() const {
        glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), position_);
        glm::mat4 rotationXMatrix = glm::rotate(glm::mat4(1.0f), rotation_.x, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotationYMatrix = glm::rotate(glm::mat4(1.0f), rotation_.y, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotationZMatrix = glm::rotate(glm::mat4(1.0f), rotation_.z, glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale_);

        return translationMatrix * rotationZMatrix * rotationYMatrix * rotationXMatrix * scaleMatrix;
    }

    // Méthode pour effectuer une translation
    void translate(const glm::vec3& translation) {
        position_ += translation;
    }

    // Méthode pour effectuer une rotation autour d'un axe donné
    void rotate(float angle, const glm::vec3& axis) {
        rotation_ += angle * axis; // Ajoute l'angle de rotation à l'axe existant
    }

    // Méthode pour effectuer une mise à l'échelle
    void scale(const glm::vec3& scaling) {
        scale_ *= scaling;
    }

    // Focntion qui permet de combiner deux transformations avec un facteur k
    Transform mix_with(const Transform& b, float k) const {
        Transform result;

        // Interpolation linéaire pour la position, la rotation et l'échelle
        result.position_ = (1.0f - k) * position_ + k * b.position_;
        result.rotation_ = (1.0f - k) * rotation_ + k * b.rotation_;
        result.scale_ = (1.0f - k) * scale_ + k * b.scale_;

        return result;
    }


    
};

#endif