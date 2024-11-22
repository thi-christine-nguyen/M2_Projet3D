#ifndef SCENE_MANAGER_HPP__
#define SCENE_MANAGER_HPP__

#include <vector>
#include <memory> // Pour std::unique_ptr
#include "lib.hpp"
#include "GameObject.hpp"

class SceneManager {
private:
    GLuint programID; 
    std::vector<std::unique_ptr<GameObject>> objects; // Vecteur de pointeurs uniques vers les objets de la scène ce qui garantit que chaque GameObject est géré par un unique SceneManager

public:
    SceneManager(GLuint programID) : programID(programID) {}

    // Méthode pour ajouter un objet à la scène
    void addObject(std::unique_ptr<GameObject> object) {
        object->setId(objects.size()); 
        objects.push_back(std::move(object)); // std::move permet de transférer la responsabilité de gestion de l'objet au SceneManager
    }

    // Méthode pour mettre à jour tous les objets de la scène
    void update(float deltaTime) {
        for (const auto& object : objects) {
            // Mettre à jour l'objet
            object->update(deltaTime);
        }
    }

    // Méthode pour afficher tous les objets de la scène
    void draw() {
        for (const auto& object : objects) {
            // Afficher l'objet
            object->draw();
        }
    }

    void initGameObjectsTexture() {
        for (const auto& object : objects) {
            // Init la texture de l'objet
            object->initTexture(programID);
        }
    }

    GameObject *getObjectByName(const std::string& name) {
        for (const auto& object : objects) {
            if (object->getName() == name)
                return object.get();
        }
        return nullptr;
    }

    std::vector<std::unique_ptr<GameObject>>& getObjects() {
        return objects;
    }

    


};

#endif