#ifndef SCENE_MANAGER_HPP__
#define SCENE_MANAGER_HPP__

#include <vector>
#include <memory> // Pour std::unique_ptr
#include "lib.hpp"
#include "GameObject.hpp"
#include "Mesh.hpp"

class SceneManager {
private:
    std::vector<std::unique_ptr<GameObject>> objects; // Vecteur de pointeurs uniques vers les objets de la scène ce qui garantit que chaque GameObject est géré par un unique SceneManager

public:
    SceneManager() {}

    // Méthode pour ajouter un objet à la scène
    void addObject(std::unique_ptr<GameObject> object);

    // Méthode pour mettre à jour tous les objets de la scène
    void update(float deltaTime);

    // Méthode pour afficher tous les objets de la scène
    void draw(Shader &shader);
    void drawVoxel(Shader &shader);

    void initGameObjectsTexture();
    GameObject *getObjectByName(const std::string& name);
    std::vector<std::unique_ptr<GameObject>>& getObjects();

};

#endif