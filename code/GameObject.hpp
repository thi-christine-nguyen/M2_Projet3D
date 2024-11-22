#ifndef GAMEOBJECT_HPP__
#define GAMEOBJECT_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// UniquePtr
#include <memory>

#include <string>
#include <cstring>
#include <cstdio>

// Include standard headers
#include "lib.hpp"
#include "Transform.hpp"
#include "BoundingBox.hpp"
#include "RegularGrid.hpp"
#include "Material.h"

enum GameObjectType { 
    DEFAULT,
    SPHERE,
    CUBE,
    PLANE,
    LANDSCAPE,
    PLAYER, 
    MESH
};

struct Triangle {
    glm::vec3 v1, v2, v3;
    glm::vec3 getNormal() const {
        glm::vec3 edge1 = v2 - v1;
        glm::vec3 edge2 = v3 - v1;
        return glm::normalize(glm::cross(edge1, edge2));
    }
    glm::vec3 getCenter() const {
        return (v1 + v2 + v3) / 3.0f;
    }
};

class GameObject {
protected:
    // GENERAL
    Transform transform;  // Transform de l'objet
    Transform initialTransform; 
    GameObjectType type; // Type du GameObject
    std::string name; // Nom et identifiant de l'objet
    int id; 

    // HIERARCHY
    std::vector<GameObject*> children;  // Liste des enfants de cet objet
    GameObject *parent; // Référence vers le parent de l'objet

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

    // PHYSICS
    BoundingBox boundingBox;
    RegularGrid grid;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float weight = 1.f;
    bool grounded = false;
    float restitutionCoef = 0.6f;
    bool hasPhysic = false; 
    bool onPlane = false; 

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
    bool gravityEnabled_ = false;
    bool isWireframe = false;

public:
    /* ------------------------- CONSTRUCTOR -------------------------*/

    // Constructeur prenant une transformation optionnelle
    GameObject(std::string name = "", int textId = 0, const Transform& initialTransform = Transform(), GameObject* parent = nullptr, GameObjectType type = GameObjectType::DEFAULT, const Material& material = Material())
        : name(name), textureID(textId), transform(initialTransform), parent(parent), type(type), material(material) {
        ptr.reset(this);
    }

    // Méthode pour ajouter un enfant à cet objet
    void addChild(GameObject* child) {
        children.push_back(child);
        child->parent = this;
    }

    
    std::vector<glm::vec3> getVerticesWorld() const{
        // Obtenez la transformation de l'objet dans l'espace mondial
        glm::mat4 worldTransform = getWorldBasedTransform();
        std::vector<glm::vec3> verticesWorld; 

        for (size_t i = 0; i < vertices.size(); ++i) {
            // Appliquez la transformation à chaque sommet
            verticesWorld.push_back(glm::vec3(worldTransform * glm::vec4(vertices[i], 1.0f)));
        }
        return verticesWorld; 
    }

    glm::vec3 getPointInWorld(glm::vec3 point){
        glm::mat4 worldTransform = getWorldBasedTransform();

        return glm::vec3(worldTransform * glm::vec4(point, 1.0f));
    }

    // Initialise la bounding box selon les vertices du GameObject
    void initBoundingBox() {
        std::vector<glm::vec3> verticesWorld = getVerticesWorld(); 
        boundingBox.init(verticesWorld);
        glm::vec3 min = boundingBox.getMin();
        glm::vec3 max = boundingBox.getMax();
        grid = RegularGrid(min, max, 10);
        grid.initBuffers();
        // std::cout << "Initialisation of bounding box done : min(" << min.x << "; " << min.y << "; " << min.z << ") / max(" << max.x << "; " << max.y << "; " << max.z << ")" << std::endl;
    }

    /* ------------------------- GETTERS/SETTERS -------------------------*/

    // Méthode pour obtenir la transformation de cet objet
    const Transform& getTransform() const {
        return transform;
    }


    // Méthode pour définir la transformation de cet objet
    void setTransform(const Transform& newTransform) {
        transform = newTransform;
        initBoundingBox();
    }

    void setInitalTransform(const Transform& newTransform) {
        initialTransform = newTransform;
        initBoundingBox();
    }

    // Méthode pour modifier la position de cet objet
    void setPosition(glm::vec3 pos) {
        transform.setPosition(pos);
        initBoundingBox();
    }

    // Méthodes pour accéder et modifier le parent de cet objet
    GameObject* getParent() const {
        return parent;
    }

    void setParent(GameObject* newParent) {
        parent = newParent;
    }

    // Méthodes pour accéder et modifier le type de cet objet
    GameObjectType getType() const {
        return type;
    }

    void setType(GameObjectType newType) {
        type = newType;
    }

    //Méthodes pour accéder et modifier le nom de cet objet

    std::string getName() const {
        return name;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    // Méthodes pour accéder et modifier la couleur de cet objet
    glm::vec4 getColor() const {
        return color;
    }

    void setColor(glm::vec4 newColor) {
        color = newColor;
    }

    // Méthodes pour accéder et modifier la vitesse de cet objet
    glm::vec3 getVelocity() const {
        return velocity;
    }

    void setVelocity(glm::vec3 newVelocity) {
        velocity = newVelocity;
    }

    // Méthodes pour accéder et modifier l'accélération de cet objet
    glm::vec3 getAcceleration() const {
        return acceleration;
    }

    void setAcceleration(glm::vec3 newAcceleration) {
        acceleration = newAcceleration;
    }

    // Méthodes pour accéder et modifier le poids de cet objet
    float getWeight() const {
        return weight;
    }

    void setWeight(float newWeight) {
        weight = newWeight;
        acceleration = glm::vec3(0.f, weight * -9.81, 0.f);
    }

    // Getter pour la boîte englobante
    const BoundingBox& getBoundingBox() const {
        return boundingBox;
    }

    // Setter pour la boîte englobante
    void setBoundingBox(const BoundingBox& bbox) {
        boundingBox = bbox;
    }

    const RegularGrid& getRegularGrid() const {
        return grid;
    }

    // Setter pour la boîte englobante
    void setRegularGrid(const RegularGrid& _grid) {
        grid = _grid;
    }

    bool getHasPhysic(){
        return hasPhysic; 
    }

    void setHasPhysic(bool physic){
        hasPhysic = physic; 
    }

    std::vector<unsigned short> getIndices() const{
        return indices; 
    }

    std::vector<glm::vec3> getVertices() const {
        return vertices; 
    }

    glm::vec3 getWorldBasedPosition() const {
        glm::vec4 pos = getWorldBasedTransform() * glm::vec4(transform.getPosition(), 1.0f);
        return glm::vec3(pos);
    }

    int setId(int _id){
        id = _id;
    }

    void setMaterial(Material m){
        material = m; 
    }


    /* ------------------------- TRANSFORMATIONS -------------------------*/

    void translate(const glm::vec3 &translation)
    {
        transform.translate(translation);
        boundingBox.updateAfterTransformation(vertices, transform.getMatrix());
        grid.updateAfterTransformation(vertices, transform.getMatrix());
    }
    void scale(const glm::vec3 &scale)
    {
        transform.scale(scale);
        boundingBox.updateAfterTransformation(vertices, transform.getMatrix());
        grid.updateAfterTransformation(vertices, transform.getMatrix());
    }
    void rotate(float angle, const glm::vec3 &axis)
    {
        transform.rotate(angle, axis);
        boundingBox.updateAfterTransformation(vertices, transform.getMatrix());
        grid.updateAfterTransformation(vertices, transform.getMatrix());
    }

    glm::mat4 getWorldBasedTransform() const {
        return parent == nullptr ? transform.getMatrix() : parent->getWorldBasedTransform() * transform.getMatrix();
    }

    /* ------------------------- BUFFERS -------------------------*/
    void GenerateBuffers(GLuint programID)
    {
        glGenVertexArrays(1, &vao);    // Le VAO qui englobe tout
        glGenBuffers(1, &vboVertices); // VBO vertex
        glGenBuffers(1, &vboUV);        // VBO uv
        glGenBuffers(1, &vboIndices);  // VBO d'élements indices pour draw triangles
        glGenBuffers(1, &vboNormals);

        transformULoc = glGetUniformLocation(programID, "model");
        typeULoc = glGetUniformLocation(programID, "type");
        colorULoc = glGetUniformLocation(programID, "color");
        textureULoc = glGetUniformLocation(programID, "gameObjectTexture");
        textureIdULoc = glGetUniformLocation(programID, "textureID");

        ambientULoc = glGetUniformLocation(programID, "material.ambient");
        diffuseULoc = glGetUniformLocation(programID, "material.diffuse");
        specularULoc = glGetUniformLocation(programID, "material.specular");
        shininessULoc = glGetUniformLocation(programID, "material.shininess");


        // Binds + Chargement des buffers avec les donnéees de l'objets courant
        // Vertices
        glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
        // UV
        glBindBuffer(GL_ARRAY_BUFFER, vboUV);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &uvs[0], GL_STATIC_DRAW);
        // Indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * indices.size(), &indices[0], GL_STATIC_DRAW);
        // Normales
        glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &normals[0], GL_STATIC_DRAW);
    }

    void DeleteBuffers(GLuint programID)
    {
        glDeleteBuffers(1, &vboVertices);
        glDeleteBuffers(1, &vboIndices);
        glDeleteBuffers(1, &vboUV);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vboNormals);
        glDeleteProgram(programID);
        for (GameObject *child : children)
        {
            child->DeleteBuffers(programID);
        }
    };

virtual void draw() const
{
    // Si Wireframe, passe en mode GL_LINE
    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // --- Dessiner l'objet principal ---
    glBindVertexArray(vao); // Bind le VAO
    glUniform1i(typeULoc, type); // Envoi du type
    glUniformMatrix4fv(transformULoc, 1, GL_FALSE, &getWorldBasedTransform()[0][0]); // Matrice de transformation
    glUniform4fv(colorULoc, 1, &color[0]); // Couleur

    // Bind et activer la texture
    glUniform1i(textureIdULoc, textureID);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureULoc, 0);

    // Attributs Vertex: Positions, UVs et Normales
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboUV);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(2);

    // Matériaux (Phong Lighting)
    glUniform3fv(ambientULoc, 1, &material.ambient_material[0]);
    glUniform3fv(diffuseULoc, 1, &material.diffuse_material[0]);
    glUniform3fv(specularULoc, 1, &material.specular_material[0]);
    glUniform1f(shininessULoc, material.shininess);

    // Bind et draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    // // Dessin récursif pour les enfants
    // for (const GameObject *child : children) {
    //     child->draw();
    // }

    // Désactiver les layouts et delink VAO
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    boundingBox.draw();
    grid.draw();
}

    /* ------------------------- TEXTURES -------------------------*/

    void initTexture(GLuint programID) {
        if (textureID != 0) { // S'il y a une texture sur le GameObject
            std::cout << textureID << ": " << texturePath << std::endl;
            glActiveTexture(GL_TEXTURE0);
            loadTexture2DFromFilePath(texturePath); 
            glUniform1i(glGetUniformLocation(programID, "gameObjectTexture"), 0);
        }
        for (GameObject *child : children) {
            child->initTexture(programID);
        }
    }
    /* ------------------------- PHYSICS -------------------------*/

    void updatePhysicallyBasedPosition(float deltaTime) {
        // Application de la force gravitationnelle
        if (gravityEnabled_  && !grounded)
            velocity += acceleration * deltaTime;

        // Application de sa vitesse à notre objet
        if (gravityEnabled_ ){
            translate(velocity * deltaTime);
        }
        
    }
    // Fonction pour calculer les impulsions à appliquer à chaque objet
    void calculer_impulsions(const GameObject& obj2, glm::vec3& impulsion1, glm::vec3& impulsion2, const glm::vec3& normale_contact) {
        // Calcul des impulsions en utilisant les lois de la physique
        // Ici, nous utilisons une approche simplifiée pour l'exemple
        float restitution_coefficient = 0.5f; // Coefficient de restitution
        float impulse_factor = (1.0f + restitution_coefficient) * glm::dot(obj2.getVelocity() - getVelocity(), normale_contact) / (1 / getWeight() + 1 / obj2.getWeight());
        impulsion1 = impulse_factor * normale_contact / getWeight();
        impulsion2 = -impulse_factor * normale_contact / obj2.getWeight();
    }

    /* ------------------------- UPDATE -------------------------*/
    // Elements à mettre à jour à chaque frame ou tick de mise à jour

    void update(float deltaTime) {
        updatePhysicallyBasedPosition(deltaTime);
    }

    std::unique_ptr<GameObject> ptr; // Pointeur unique vers l'objet

    /* ------------------------- INTERFACE -------------------------*/

    void resetParameters() {
        // Réinitialiser la transformation à sa valeur par défaut
        transform = initialTransform; 

        // Réinitialiser d'autres paramètres selon vos besoins
        scaleLocked_ = false;
        gravityEnabled_ = false;
        velocity = glm::vec3(0.0f); 
    }

    void updateInterfaceTransform(float _deltaTime) {

        
        ImGui::Text("Position");
        glm::vec3 position = transform.getPosition();
        ImGui::DragFloat3(("##" + std::to_string(id) + "Position").c_str(), glm::value_ptr(position));

        glm::vec3 rotation = transform.getRotation();
        ImGui::Text("Rotation");
        ImGui::DragFloat3(("##" + std::to_string(id) + "Rotation").c_str(), glm::value_ptr(rotation));

        glm::vec3 scale = transform.getScale();
        ImGui::Text("Lock Scale");
        ImGui::SameLine();
        ImGui::Checkbox(("##" + std::to_string(id) + "LockScale").c_str(), &scaleLocked_);

        if (scaleLocked_) {
            ImGui::Text("Scale");
            ImGui::DragFloat((std::string("##") + std::to_string(id) + "Scale").c_str(), &scale.x, 0.1f, 0.0f, FLT_MAX);
            scale.y = scale.x;
            scale.z = scale.x;
        } else {
            ImGui::Text("Scale x, y, z");
            ImGui::DragFloat3((std::string("##") + std::to_string(id) + "Scale").c_str(), glm::value_ptr(scale), 0.1f, 0.0f, FLT_MAX);
        }

        if (hasPhysic) {
            ImGui::Text("Gravity Enabled");
            ImGui::SameLine();
            ImGui::Checkbox(("##" + std::to_string(id) + " GravityEnabled").c_str(), &gravityEnabled_);
        }
        ImGui::Checkbox(("Afficher en Wireframe ##" + std::to_string(id)).c_str(), &isWireframe);


        transform.setPosition(position);
        transform.setRotation(rotation);
        transform.setScale(scale);

        if (ImGui::Button(("Reset " + std::to_string(id) + " Parameters").c_str())) {
            resetParameters();
        }
    }



};

#endif