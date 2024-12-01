#include "SceneManager.hpp"

// Méthode pour ajouter un objet à la scène
void SceneManager::addObject(std::unique_ptr<GameObject> object) {
    object->setId(objects.size()); 
    objects.push_back(std::move(object)); // std::move permet de transférer la responsabilité de gestion de l'objet au SceneManager
}

// Méthode pour mettre à jour tous les objets de la scène
void SceneManager::update(float deltaTime) {
    for (const auto& object : objects) {
        // Mettre à jour l'objet
        object->update(deltaTime);
    }
}

// Méthode pour afficher tous les objets de la scène
void SceneManager::draw(Shader &shader) {
    for (const auto& object : objects) {
        // Afficher l'objet
        object->draw(shader);
    }
}

void SceneManager::initGameObjectsTexture() {
    for (const auto& object : objects) {
        // Init la texture de l'objet
        object->initTexture();
    }
}

GameObject *SceneManager::getObjectByName(const std::string& name) {
    for (const auto& object : objects) {
        if (object->getName() == name)
            return object.get();
    }
    return nullptr;
}

std::vector<std::unique_ptr<GameObject>>& SceneManager::getObjects() {
    return objects;
}