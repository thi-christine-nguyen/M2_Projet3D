#ifndef GAMEOBJECT_HPP__
#define GAMEOBJECT_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// UniquePtr
#include <memory>
#include <string>
#include <cstring>
#include <cstdio>

#include "Transform.hpp"
#include "Material.h"
#include "Shader.hpp"
#include "texture.hpp"
#include "RegularGrid.hpp"
#include "AdaptativeGrid.hpp"

class GameObject {
protected:
    // GENERAL
    Transform transform;  // Transform de l'objet
    Transform initialTransform; 
    std::string name;
    int id; 
    Shader shader; 

    // BUFFERS
    GLuint vao;
    GLuint vboVertices;
    GLuint vboIndices;
    GLuint vboUV;
    GLuint vboNormals; 

    // UNIFORM LOCATION
    GLuint typeULoc;
    GLuint transformULoc;
    GLuint colorULoc;
    GLuint textureULoc;
    GLuint textureIdULoc;

    // MESH DATA
    std::vector<unsigned short> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    glm::vec4 color {1.f, 1.f, 1.f, 1.f};

    // TEXTURE
    int textureID; // 0 = flatColor sinon Texture
    const char *texturePath;

    // // MATERIAL 
    Material material; 
    GLuint ambientULoc;
    GLuint diffuseULoc;
    GLuint specularULoc;
    GLuint shininessULoc;

    // INTERFACE
    bool scaleLocked_ = false; 
    bool isWireframe = false;
    bool isWireframeVoxel = false; 
    bool showMesh = true; 
    bool showVoxel = false; 

    AdaptativeGrid grid;
    int voxelResolution = 0; 
    bool gridInitialized = false;


public:
    std::unique_ptr<GameObject> ptr;
    /* ------------------------- CONSTRUCTOR -------------------------*/

    // Constructeur prenant une transformation optionnelle
    GameObject(std::string name, int textId, const Transform& initialTransform, const Material& material, const Shader &shader);

    /* ------------------------- GETTERS/SETTERS -------------------------*/

    // Méthode pour obtenir la transformation de cet objet
    const Transform& getTransform() const;

    // Méthode pour définir la transformation de cet objet
    void setTransform(const Transform& newTransform);
    void setInitalTransform(const Transform& newTransform);

    // Méthode pour modifier la position de cet objet
    void setPosition(glm::vec3 pos);

    //Méthodes pour accéder et modifier le nom de cet objet
    std::string getName() const;
    void setName(const std::string& newName);

    // Méthodes pour accéder et modifier la couleur de cet objet
    glm::vec4 getColor() const;
    void setColor(glm::vec4 newColor);
    std::vector<unsigned short> getIndices() const;
    std::vector<glm::vec3> getVertices() const;
    int setId(int _id);
    void setMaterial(const Material& _material);
    void setAmbient(glm::vec3 _ambient);

    /* ----------------------------- UPDATE -----------------------------*/
    void update(float deltaTime);

    /* ------------------------- TRANSFORMATIONS -------------------------*/
    void translate(const glm::vec3 &translation);
    void scale(const glm::vec3 &scale);
    void rotate(float angle, const glm::vec3 &axis);

    /* ------------------------- BUFFERS -------------------------*/
    void GenerateBuffers();
    void DeleteBuffers();

    void draw(Shader &shader);
    void drawVoxel(Shader &shader);

    /* ------------------------- TEXTURES -------------------------*/
    void initTexture();

    /* ------------------------- INTERFACE -------------------------*/
    void resetParameters();
    void updateInterfaceTransform(float _deltaTime);

    bool getIsWireframe();
    bool getIsWireframeVoxel(); 
    bool getShowMesh(); 
    bool getShowVoxel(); 


    /* ------------------------- DESTRUCTOR -------------------------*/
    virtual ~GameObject() = default;
};

#endif