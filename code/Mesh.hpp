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

class Mesh : public GameObject {
public:
    bool editMode = false;
    char newName[128]; 
    std::string meshPath; 
    std::string newtexturePath;
    GLuint programID; 

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

 

    void updateInterfaceTransform(float _deltaTime){
        
        
        ImGui::Checkbox(("##" + std::to_string(id) + " Mode édition").c_str(), &editMode);
        if (editMode) {
            
            ImGui::Text("Edition des Propriétés de l'Objet");
            
            // Modification du nom
            ImGui::Text("Nom");
            ImGui::InputText(("##" + std::to_string(id) + " Nom").c_str(), newName, IM_ARRAYSIZE(newName));
            if (ImGui::Button(("Valider le nom ##" + std::to_string(id)).c_str())){
                name = newName; 
            }

            // Modification du mesh 
            if (ImGui::Button("Sélectionner le Mesh")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog("ChooseMeshDlgKey", "Choose Mesh File", ".obj,.off", config);
            }

            // Afficher la boîte de dialogue pour choisir le fichier de mesh
            if (ImGuiFileDialog::Instance()->Display("ChooseMeshDlgKey")) {
                if (ImGuiFileDialog::Instance()->IsOk()) {
                    meshPath = ImGuiFileDialog::Instance()->GetFilePathName(); // Obtenir le chemin du fichier sélectionné
                    indices.clear();
                    vertices.clear();
                    uvs.clear();
                    normals.clear();
                    // this->DeleteBuffers(programID); 
                    loadModel(meshPath.c_str()); 
                    this->GenerateBuffers(programID);
                    this->initBoundingBox();
                }
                ImGuiFileDialog::Instance()->Close();
            }

            ImGui::Text("Selected Mesh File: %s", meshPath.c_str());



            // std::string texture; 
            // // Chemin pour la texture
            // if (ImGui::Button("Texture Path")) {
            //     IGFD::FileDialogConfig config;
            //     config.path = ".";
            //     ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgTextureKey", "Choose File", ".png, .jpg, .bmp", config);
            // }
            // if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgTextureKey")) {
            //     if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                    
            //         texture = ImGuiFileDialog::Instance()->GetFilePathName();
                    
            //         std::cout << texture << std::endl; 
            //         GLuint textureID = loadTexture2DFromFilePath(texture);
            //         texturePath = texture.c_str(); 
            //         initTexture(programID);
                 
                    
            //     }
            //     ImGuiFileDialog::Instance()->Close();
            // }
            // // if(texturePath != ""){

            // //     if (ImGui::Button("Annuler le texture")){
            // //         texturePath = ""; 
            // //     }

            // // }

            // ImGui::Text("Selected Texture File: %s", texture.c_str());

            // if (newtexturePath.empty()) {
            //     ImGui::Text("Couleur RGB (0-256)");
            //     static float colorWheel[3] = {1.0f, 1.0f, 1.0f};  // Valeurs normalisées de 0 à 1
            //     static bool colorPopupOpen = false;

            //     // Bouton pour ouvrir la roue de couleurs
            //     if (ImGui::Button("Choisir une couleur")) {
            //         ImGui::OpenPopup("ColorPickerPopup");
            //     }

            //     // Pop-up de sélection de couleur
            //     if (ImGui::BeginPopup("ColorPickerPopup")) {
            //         ImGui::Text("Sélectionnez une couleur");
            //         ImGui::Separator();

            //         // Affiche la roue de couleur
            //         ImGui::ColorPicker3("Couleur", colorWheel);

            //         ImGui::Separator();
            //         if (ImGui::Button("OK", ImVec2(120, 0))) {
            //             color[0] = colorWheel[0]; 
            //             color[1] = colorWheel[1]; 
            //             color[2] = colorWheel[2]; 
            //             ImGui::CloseCurrentPopup();
            //         }

            //         ImGui::EndPopup();
            //     }

            //     int colorRGB[3] = {
            //         static_cast<int>(color[0] * 255),
            //         static_cast<int>(color[1] * 255),
            //         static_cast<int>(color[2] * 255)
            //     };
            //     ImGui::Text("Couleur sélectionnée : R%d G%d B%d", colorRGB[0], colorRGB[1], colorRGB[2]);
            // }


            
            // // Sélection du chemin du mesh
            // char meshPathBuffer[256];
            // strncpy(meshPathBuffer, meshPath.c_str(), sizeof(meshPathBuffer));
            // if (ImGui::InputText(("Chemin du Mesh##"  + std::to_string(id)).c_str(), meshPathBuffer, sizeof(meshPathBuffer))) {
            //     meshPath = std::string(meshPathBuffer);
            // }

            // // Sélection du chemin de la texture
            // static char texturePathBuffer[256];
            // strncpy(texturePathBuffer, texturePath.c_str(), sizeof(texturePathBuffer));
            // if (ImGui::InputText("Chemin de la Texture", texturePathBuffer, sizeof(texturePathBuffer))) {
            //     texturePath = std::string(texturePathBuffer);
            // }

            // // Sélection de la couleur si pas de texture
            // if (texturePath.empty()) {
            //     ImGui::Text("Couleur RGB");
            //     static float colorWheel[3] = {1.0f, 1.0f, 1.0f};
            //     if (ImGui::ColorEdit3("Couleur", colorWheel)) {
            //         color[0] = colorWheel[0];
            //         color[1] = colorWheel[1];
            //         color[2] = colorWheel[2];
            //     }
            // }
        }
        ImGui::Separator();
        GameObject::updateInterfaceTransform(_deltaTime);
        
        
        
    }

};

#endif