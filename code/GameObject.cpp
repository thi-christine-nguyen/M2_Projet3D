#include "GameObject.hpp"
/* ------------------------- CONSTRUCTOR -------------------------*/

// Constructeur prenant une transformation optionnelle
GameObject::GameObject(std::string name = "", int textId = 0, const Transform& initialTransform = Transform(), const Material& _material = Material(), const Shader &shader = Shader("", ""))
    : name(name), textureID(textId), transform(initialTransform), material(_material), shader(shader) {
        ptr.reset(this);
}

/* ------------------------- GETTERS/SETTERS -------------------------*/

// Méthode pour obtenir la transformation de cet objet
const Transform& GameObject::getTransform() const {
    return transform;
}

// Méthode pour définir la transformation de cet objet
void GameObject::setTransform(const Transform& newTransform) {
    transform = newTransform;
}

void GameObject::setInitalTransform(const Transform& newTransform) {
    initialTransform = newTransform;
}

// Méthode pour modifier la position de cet objet
void GameObject::setPosition(glm::vec3 pos) {
    transform.setPosition(pos);
}

//Méthodes pour accéder et modifier le nom de cet objet

std::string GameObject::getName() const {
    return name;
}

void GameObject::setName(const std::string& newName) {
    name = newName;
}

// Méthodes pour accéder et modifier la couleur de cet objet
glm::vec4 GameObject::getColor() const {
    return color;
}

void GameObject::setColor(glm::vec4 newColor) {
    color = newColor;
}

std::vector<unsigned short> GameObject::getIndices() const{
    return indices; 
}

std::vector<glm::vec3> GameObject::getVertices() const {
    return vertices; 
}

int GameObject::setId(int _id){
    id = _id;
}

void GameObject::setMaterial(const Material& _material){
    material = _material; 
}

void GameObject::setAmbient(glm::vec3 _ambient) {
    material.setAmbient(_ambient);
}

/* ------------------------- TRANSFORMATIONS -------------------------*/

void GameObject::translate(const glm::vec3 &translation)
{
    transform.translate(translation);
}
void GameObject::scale(const glm::vec3 &scale)
{
    transform.scale(scale);
}
void GameObject::rotate(float angle, const glm::vec3 &axis)
{
    transform.rotate(angle, axis);
}

/* ------------------------- BUFFERS -------------------------*/
void GameObject::GenerateBuffers()
{
    glGenVertexArrays(1, &vao);    // Le VAO qui englobe tout
    glGenBuffers(1, &vboVertices); // VBO vertex
    glGenBuffers(1, &vboUV);        // VBO uv
    glGenBuffers(1, &vboIndices);  // VBO d'élements indices pour draw triangles
    glGenBuffers(1, &vboNormals);

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

void GameObject::DeleteBuffers()
{
    glDeleteBuffers(1, &vboVertices);
    glDeleteBuffers(1, &vboIndices);
    glDeleteBuffers(1, &vboUV);
    glDeleteBuffers(1, &vboNormals);
    glDeleteVertexArrays(1, &vao);
};

void GameObject::draw(Shader &shader)
{
    // Si Wireframe, passe en mode GL_LINE
    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
     // Matériaux (Phong Lighting)
    glUniform3fv(glGetUniformLocation(shader.ID, "material.ambient"), 1, glm::value_ptr(material.getAmbient()));
    glUniform3fv(glGetUniformLocation(shader.ID, "material.diffuse"), 1, glm::value_ptr(material.getDiffuse()));
    glUniform3fv(glGetUniformLocation(shader.ID, "material.specular"), 1, glm::value_ptr(material.getSpecular()));
    glUniform1f(glGetUniformLocation(shader.ID, "material.shininess"), material.getShininess());

    // --- Dessiner l'objet principal ---
    glBindVertexArray(vao); // Bind le VAO
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, &transform.getMatrix()[0][0]); // Matrice de transformation
    glUniform4fv(glGetUniformLocation(shader.ID, "color"), 1, &color[0]); // Couleur

    // Bind et activer la texture
    glUniform1i(glGetUniformLocation(shader.ID, "textureID"), textureID);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader.ID, "gameObjectTexture"), 0);
    glBindTexture(GL_TEXTURE_2D, textureID);

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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

    // Désactiver les layouts et delink VAO
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

/* ------------------------- UPDATE -------------------------*/
void GameObject::update(float deltaTime)
{
}


/* ------------------------- TEXTURES -------------------------*/

void GameObject::initTexture() {
    if (textureID != 0) { // S'il y a une texture sur le GameObject
        std::cout << textureID << ": " << texturePath << std::endl;
        glActiveTexture(GL_TEXTURE0);
        loadTexture2DFromFilePath(texturePath); 
        glUniform1i(glGetUniformLocation(shader.ID, "gameObjectTexture"), 0);
    }
    
}


/* ------------------------- INTERFACE -------------------------*/

void GameObject::resetParameters() {
    transform = initialTransform; 
    scaleLocked_ = false;
}

void GameObject::updateInterfaceTransform(float _deltaTime) {

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

    ImGui::Checkbox(("Afficher en Wireframe ##" + std::to_string(id)).c_str(), &isWireframe);


    transform.setPosition(position);
    transform.setRotation(rotation);
    transform.setScale(scale);
    

    if (ImGui::Button(("Reset " + std::to_string(id) + " Parameters").c_str())) {
        resetParameters();
    }
}