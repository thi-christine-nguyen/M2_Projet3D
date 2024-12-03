#ifndef MESH_HPP__
#define MESH_HPP__
#include "GameObject.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Shader.hpp"
#include "Interface.hpp"
#include "RegularGrid.hpp"

class Mesh : public GameObject {
private :

    bool editMode = false;
    char newName[128]; 
    std::string meshPath; 
    std::string newtexturePath;
    RegularGrid grid;

public :

    Mesh(std::string name, const char *path, int textureID, const char *texturePath, Shader &shader);
    Mesh(std::string name, const char *path, glm::vec4 color, Shader &shader);
    RegularGrid& getGrid() {
        return grid;
    }
    void loadModel(const char *path);
    bool loadOBJ(const char *path);
    bool loadOFF(const char *path);
    void updateInterfaceTransform(float _deltaTime);
    void draw(Shader &shader);

};
#endif