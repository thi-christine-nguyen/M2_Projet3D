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
#include <queue>
#include <unordered_set>

struct Voxel {
    glm::vec3 position; // Position du voxel dans l'espace
    glm::vec3 color;    // Couleur du voxel
    float size;         // Taille du voxel
};


class Mesh : public GameObject {

public:
    bool editMode = false;
    char newName[128]; 
    std::string meshPath; 
    std::string newtexturePath;
    GLuint programID; 

    std::vector<glm::vec3> vertex; 
    
    // Voxelisation
    std::vector<std::vector<std::vector<int>>> grid;
    std::vector<glm::vec3> voxelVerticesGrid; // Verts des cubes (voxels)
    std::vector<GLuint> voxelIndicesGrid; // Indices pour les triangles du cube
    GLuint vaoVoxels, vboVoxelVertices, vboVoxelIndices;
    std::vector<Voxel> voxels;
    bool showMesh = true;
    int gridSizeX; 
    int gridSizeY; 
    int gridSizeZ; 




    Mesh() {}

    Mesh(std::string name, const char *path, int textureID, const char *texturePath, GLuint programID = 0) {
        this->name = name;
        strncpy(newName, name.c_str(), sizeof(newName) - 1); 
        this->type = MESH;
        this->textureID = textureID;
        this->texturePath = texturePath;

        indices.clear();
        vertices.clear();
        uvs.clear();
        normals.clear();
        
        loadModel(path);
        meshPath = path; 
        newtexturePath = texturePath; 
        this->programID = programID; 
        
        this->GenerateBuffers(programID);
        this->initBoundingBox();
        vertex = vertices; 
        // drawGrid(boundingBox.getMin(), boundingBox.getMax(), 1);        
        // generateBoundingBoxVertices();  
        generateVoxelGrid(0.5); 
      
    }

    void loadModel(const char *path) {
        std::string extension = std::string(path).substr(std::string(path).find_last_of(".") + 1);
  
        if (extension == "obj") {
            if (!loadOBJ(path)) {
                std::cerr << "Failed to load OBJ model." << std::endl;
            }
        } else if (extension == "off") {
            if (!loadOFF(path)) {
                std::cerr << "Failed to load OFF model." << std::endl;
            }
        } else {
            std::cerr << "Unsupported file format." << std::endl;
        }
    }

    bool loadOBJ(const char *path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::vector<glm::vec3> temp_vertices;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            std::string type;
            ss >> type;
            

            if (type == "v") {
                glm::vec3 position;
                
                ss >> position.x >> position.y >> position.z;
                vertices.push_back(position);
            }
            else if (type == "vn") {
                glm::vec3 normal;
                ss >> normal.x >> normal.y >> normal.z;
               
                normals.push_back(normal);
            }
            else if (type == "vt") {
                glm::vec2 texCoord;
                ss >> texCoord.x >> texCoord.y;
            
                uvs.push_back(texCoord);
            }
            else if (type == "f") {
                unsigned int posIdx[3], uvIdx[3], normIdx[3];
                char slash;
                for (int i = 0; i < 3; i++) {
                    ss >> posIdx[i] >> slash >> uvIdx[i] >> slash >> normIdx[i];
                    posIdx[i]--; uvIdx[i]--; normIdx[i]--;
                    indices.push_back(posIdx[i]);
                }
            }
        }

      

        file.close();
        return true;
    }


    bool loadOFF(const char *path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << path << std::endl;
            return false;
        }

        std::string header;
        file >> header;
        if (header != "OFF") {
            std::cerr << "Not a valid OFF file." << std::endl;
            return false;
        }

        int vertexCount, faceCount, edgeCount;
        file >> vertexCount >> faceCount >> edgeCount;

        vertices.reserve(vertexCount);
        for (int i = 0; i < vertexCount; i++) {
            glm::vec3 position;
            file >> position.x >> position.y >> position.z;
            vertices.push_back(position);
        }
        normals.resize(vertices.size(), glm::vec3(0.0f));

        for (int i = 0; i < faceCount; i++) {
            int numVertices;
            file >> numVertices;
            if (numVertices != 3) {
                std::cerr << "Only triangular faces are supported." << std::endl;
                return false;
            }
            unsigned int faceIdx[3];
            file >> faceIdx[0] >> faceIdx[1] >> faceIdx[2];
            indices.push_back(faceIdx[0]);
            indices.push_back(faceIdx[1]);
            indices.push_back(faceIdx[2]);

            glm::vec3 v0 = vertices[faceIdx[0]];
            glm::vec3 v1 = vertices[faceIdx[1]];
            glm::vec3 v2 = vertices[faceIdx[2]];

            glm::vec3 edge1 = v1 - v0;
            glm::vec3 edge2 = v2 - v0;
            glm::vec3 faceNormal = glm::normalize(glm::cross(edge1, edge2));

            // // Ajouter la normale aux sommets
            normals[faceIdx[0]] += faceNormal;
            normals[faceIdx[1]] += faceNormal;
            normals[faceIdx[2]] += faceNormal;
        }

        for (auto &normal : normals) {
            normal = glm::normalize(normal);
        }

          

        for (int i = 0; i < vertexCount; i++) {
            glm::vec3 position = vertices[i];
            // Exemple simple : on utilise la position en X et Y pour définir les coordonnées UV
            float u = (position.x + 1.0f) / 2.0f; // Normalisation pour l'UV
            float v = (position.y + 1.0f) / 2.0f;
            uvs.push_back(glm::vec2(u, v)); // Ajouter la coordonnée UV
        }

        file.close();

     
        return true;
    }
    

    void generateBoundingBoxVertices() {
        vertices.clear();

        // Ajouter les 8 sommets de la boîte
        vertices.push_back(glm::vec3(boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMin().z)); // 0
        vertices.push_back(glm::vec3(boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMin().z)); // 1
        vertices.push_back(glm::vec3(boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMin().z)); // 2
        vertices.push_back(glm::vec3(boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMin().z)); // 3
        vertices.push_back(glm::vec3(boundingBox.getMin().x, boundingBox.getMin().y, boundingBox.getMax().z)); // 4
        vertices.push_back(glm::vec3(boundingBox.getMax().x, boundingBox.getMin().y, boundingBox.getMax().z)); // 5
        vertices.push_back(glm::vec3(boundingBox.getMax().x, boundingBox.getMax().y, boundingBox.getMax().z)); // 6
        vertices.push_back(glm::vec3(boundingBox.getMin().x, boundingBox.getMax().y, boundingBox.getMax().z)); // 7

        indices.clear();

        // Bottom face (z = minBound.z), 2 triangles
        indices.push_back(0); indices.push_back(1); indices.push_back(2);
        indices.push_back(0); indices.push_back(2); indices.push_back(3);

        // Top face (z = maxBound.z), 2 triangles
        indices.push_back(4); indices.push_back(5); indices.push_back(6);
        indices.push_back(4); indices.push_back(6); indices.push_back(7);

        // Front face (y = minBound.y), 2 triangles
        indices.push_back(0); indices.push_back(1); indices.push_back(5);
        indices.push_back(0); indices.push_back(5); indices.push_back(4);

        // Back face (y = maxBound.y), 2 triangles
        indices.push_back(3); indices.push_back(2); indices.push_back(6);
        indices.push_back(3); indices.push_back(6); indices.push_back(7);

        // Left face (x = minBound.x), 2 triangles
        indices.push_back(0); indices.push_back(3); indices.push_back(7);
        indices.push_back(0); indices.push_back(7); indices.push_back(4);

        // Right face (x = maxBound.x), 2 triangles
        indices.push_back(1); indices.push_back(2); indices.push_back(6);
        indices.push_back(1); indices.push_back(6); indices.push_back(5);

        this->GenerateBuffers(programID);
    }

    
    bool isVertexInsideVoxel(const glm::vec3& vertex, const glm::vec3& voxelMin, const glm::vec3& voxelMax) {
        return (vertex.x >= voxelMin.x && vertex.x <= voxelMax.x) &&
            (vertex.y >= voxelMin.y && vertex.y <= voxelMax.y) &&
            (vertex.z >= voxelMin.z && vertex.z <= voxelMax.z);
    }

    void generateVoxelGrid(float voxelSize) {
        vertices.clear();
        indices.clear();

        // Résolution de la grille (nombre de voxels dans chaque direction)
        int numVoxelsX = (boundingBox.getMax().x - boundingBox.getMin().x) / voxelSize;
        int numVoxelsY = (boundingBox.getMax().y - boundingBox.getMin().y) / voxelSize;
        int numVoxelsZ = (boundingBox.getMax().z - boundingBox.getMin().z) / voxelSize;

        // Parcours de la grille de voxels
        for (int i = 0; i < numVoxelsX; ++i) {
            for (int j = 0; j < numVoxelsY; ++j) {
                for (int k = 0; k < numVoxelsZ; ++k) {
                    glm::vec3 voxelMin = glm::vec3(boundingBox.getMin().x + i * voxelSize, 
                                                    boundingBox.getMin().y + j * voxelSize, 
                                                    boundingBox.getMin().z + k * voxelSize);
                    glm::vec3 voxelMax = voxelMin + glm::vec3(voxelSize, voxelSize, voxelSize);

                    // Vérifier si un vertex du maillage est à l'intérieur de ce voxel
                    for (const auto& v : vertex) {
                        if (isVertexInsideVoxel(v, voxelMin, voxelMax)) {
                            // Si un vertex est à l'intérieur du voxel, afficher ce voxel
                            // Ajouter les 8 sommets du voxel
                            int baseIndex = vertices.size();
                            vertices.push_back(glm::vec3(voxelMin.x, voxelMin.y, voxelMin.z)); // 0
                            vertices.push_back(glm::vec3(voxelMax.x, voxelMin.y, voxelMin.z)); // 1
                            vertices.push_back(glm::vec3(voxelMax.x, voxelMax.y, voxelMin.z)); // 2
                            vertices.push_back(glm::vec3(voxelMin.x, voxelMax.y, voxelMin.z)); // 3
                            vertices.push_back(glm::vec3(voxelMin.x, voxelMin.y, voxelMax.z)); // 4
                            vertices.push_back(glm::vec3(voxelMax.x, voxelMin.y, voxelMax.z)); // 5
                            vertices.push_back(glm::vec3(voxelMax.x, voxelMax.y, voxelMax.z)); // 6
                            vertices.push_back(glm::vec3(voxelMin.x, voxelMax.y, voxelMax.z)); // 7

                            // Ajouter les indices pour les faces du voxel
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 2);
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 2); indices.push_back(baseIndex + 3);
                            indices.push_back(baseIndex + 4); indices.push_back(baseIndex + 5); indices.push_back(baseIndex + 6);
                            indices.push_back(baseIndex + 4); indices.push_back(baseIndex + 6); indices.push_back(baseIndex + 7);
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 5);
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 5); indices.push_back(baseIndex + 4);
                            indices.push_back(baseIndex + 3); indices.push_back(baseIndex + 2); indices.push_back(baseIndex + 6);
                            indices.push_back(baseIndex + 3); indices.push_back(baseIndex + 6); indices.push_back(baseIndex + 7);
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 3); indices.push_back(baseIndex + 7);
                            indices.push_back(baseIndex + 0); indices.push_back(baseIndex + 7); indices.push_back(baseIndex + 4);
                            indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 2); indices.push_back(baseIndex + 6);
                            indices.push_back(baseIndex + 1); indices.push_back(baseIndex + 6); indices.push_back(baseIndex + 5);
                        }
                    }
                }
            }
        }

        // Générer les buffers pour OpenGL
        this->GenerateBuffers(programID);
    }


    void updateInterfaceTransform(float _deltaTime){
      
        
        ImGui::Checkbox(("Edit mode ##" + std::to_string(id) ).c_str(), &editMode);
        if (editMode) {
            
            // Modification du name
            ImGui::Text("Name");
            ImGui::InputText(("##" + std::to_string(id) + " Name").c_str(), newName, IM_ARRAYSIZE(newName));
            if (ImGui::Button(("Validate name ##" + std::to_string(id)).c_str())){
                name = newName; 
            }

            // Modification du mesh 
            if (ImGui::Button(("Select Mesh ##" + std::to_string(id)).c_str())) {
                IGFD::FileDialogConfig config;
                config.path = "../data/meshes";
                ImGuiFileDialog::Instance()->OpenDialog(("##" + std::to_string(id) + " Mesh").c_str(), "Choose Mesh File", ".obj,.off", config);
            }

            if (ImGuiFileDialog::Instance()->Display(("##" + std::to_string(id) + " Mesh").c_str())) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    meshPath = ImGuiFileDialog::Instance()->GetFilePathName();
                    indices.clear();
                    vertices.clear();
                    uvs.clear();
                    normals.clear();
                    loadModel(meshPath.c_str());
                    this->GenerateBuffers(programID);
                    this->initBoundingBox();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Text("Selected Mesh File: %s", meshPath.c_str());

            // Modification de la texture
            if (ImGui::Button(("Select Texture ##" + std::to_string(id)).c_str())) {
                IGFD::FileDialogConfig config;
                config.path = "../data/textures";
                ImGuiFileDialog::Instance()->OpenDialog(("##" + std::to_string(id) + " Texture").c_str(), "Choose Texture File", ".png,.jpg,.bmp", config);
            }

            if (ImGuiFileDialog::Instance()->Display(("##" + std::to_string(id) + " Texture").c_str())) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    newtexturePath = ImGuiFileDialog::Instance()->GetFilePathName();
                    textureID = loadTexture2DFromFilePath(newtexturePath.c_str());
                    glUniform1i(glGetUniformLocation(programID, "gameObjectTexture"), 0);
                }
                ImGuiFileDialog::Instance()->Close();
            }

            if(texturePath != "" ){
                if (ImGui::Button(("Cancel texture ##" + std::to_string(id)).c_str())){
                    newtexturePath = ""; 
                    textureID = 0; 
                }
            }

            ImGui::Text("Selected Texture File: %s", newtexturePath.c_str());

            if (newtexturePath.empty() || textureID == 0) {
                ImGui::Text("Color RGB (0-256)");
                static float colorWheel[3] = {1.0f, 1.0f, 1.0f};  // Valeurs normalisées de 0 à 1
                static bool colorPopupOpen = false;

                // Bouton pour ouvrir la roue de couleurs
                if (ImGui::Button(("Choose a color ##" + std::to_string(id)).c_str())) {
                    ImGui::OpenPopup(("ColorPickerPopup ##" + std::to_string(id)).c_str());
                }

                // Pop-up de sélection de couleur
                if (ImGui::BeginPopup(("ColorPickerPopup ##" + std::to_string(id)).c_str())) {
                    ImGui::Text("Choose a color");
                    ImGui::Separator();

                    // Affiche la roue de couleur
                    ImGui::ColorPicker3(("Color ##" + std::to_string(id)).c_str(), colorWheel);

                    ImGui::Separator();
                    if (ImGui::Button(("OK ##" + std::to_string(id)).c_str(), ImVec2(120, 0))) {
                        color[0] = colorWheel[0]; 
                        color[1] = colorWheel[1]; 
                        color[2] = colorWheel[2]; 
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
                int colorRGB[3] = {
                    static_cast<int>(color[0] * 255),
                    static_cast<int>(color[1] * 255),
                    static_cast<int>(color[2] * 255)
                };
                ImGui::Text("Selected Color : R%d G%d B%d", colorRGB[0], colorRGB[1], colorRGB[2]);
            }

            ImGui::Text("Material Settings");

            glm::vec3 ambient = material.getAmbient();
            if (ImGui::DragFloat3(("Ambient ##" + std::to_string(id)).c_str(), glm::value_ptr(ambient), 0.001f)) {
                material.setAmbient(ambient);
            }
            glm::vec3 diffuse = material.getDiffuse();
            if (ImGui::DragFloat3(("Diffuse ##" + std::to_string(id)).c_str(), glm::value_ptr(diffuse), 0.001f)) {
                material.setDiffuse(diffuse);
            }
            glm::vec3 specular = material.getSpecular();
            if (ImGui::DragFloat3(("Specular ##" + std::to_string(id)).c_str(), glm::value_ptr(specular), 0.001f)) {
                material.setSpecular(specular);
            }
            // Shininess
            float shininess = material.getShininess();
            if (ImGui::SliderFloat(("Shininess ##" + std::to_string(id)).c_str(), &shininess, 1.0f, 128.0f)) {
                material.setShininess(shininess);
            }

            if (ImGui::Button("Toggle Mesh Visibility")) {
                showMesh = !showMesh;  // Inverser l'état de la visibilité du maillage
            }
            
            ImGui::Text("Voxélisation");
            static float voxelSize = 0.1f;  // Taille du voxel par défaut
            ImGui::SliderFloat("Taille du voxel", &voxelSize, 0.01f, 1.0f);  // Slider pour ajuster la taille des voxels

            // // Bouton pour générer les voxels
            // if (ImGui::Button("Générer voxels")) {
            //     // Appeler la fonction pour calculer la voxélisation
            //     voxelizeMesh(voxelSize);  // Génère la voxélisation du maillage
            //     generateVoxelData(voxelSize);  // Calcule les données des voxels (vertices et indices)
            //     initializeVoxelBuffers();  // Crée les buffers OpenGL pour les voxels
            // }
                
        }
        ImGui::Separator();
        GameObject::updateInterfaceTransform(_deltaTime);
        
    }

};

#endif