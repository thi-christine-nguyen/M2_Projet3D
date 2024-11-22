#ifndef INTERFACE_HPP__
#define INTERFACE_HPP__

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/ImGuiFileDialog.h>
#include <iostream>
#include <vector>
#include "code/Camera/Camera.hpp"
#include "SceneManager.hpp"
#include "InputManager.hpp"
#include "Mesh.hpp"



class Interface
{
private : 
    GLuint programID;

public : 

    Camera *camera; 
    SceneManager *SM;
    InputManager *IM; 
    

    Interface(GLuint _programID, SceneManager *_SM, InputManager *_IM, Camera *_camera) :
    programID(_programID),
    SM(_SM),
    IM(_IM),
    camera(_camera) {}

    void initImgui(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void createFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Positionner la fenêtre ImGui à droite
        ImGuiIO& io = ImGui::GetIO();
        float interfaceWidth = 700.0f; // Largeur souhaitée pour l'interface
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - interfaceWidth, 0), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(interfaceWidth, io.DisplaySize.y), ImGuiCond_Always);

        // Commencer une nouvelle fenêtre ImGui avec le défilement activé
        ImGui::Begin("Interface", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysUseWindowPadding);

        // Fin de la fenêtre
        ImGui::End();
    }

    

    void renderFrame(){
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Deletes all ImGUI instances
    void deleteFrame(){
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void addGameObject(float _deltaTime, GLFWwindow* _window){
        static char name[128]; 
        if (name[0] == '\0') {
            std::snprintf(name, sizeof(name), "Objet %zu", SM->getObjects().size() + 1);
        }

        static int resolution;
        static int size;
        static std::string meshPath;
        static std::string texturePath;
        static bool playable = false; 
        static bool physic = false; 
        static float poids = 0; 
        static Transform transform; 
        static bool scaleLocked_ = false; 
        static glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
        static Material material; 

        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

        // Chemin pour le mesh
        if (ImGui::Button("Select Mesh")) {
            IGFD::FileDialogConfig config;
            config.path = "../data/meshes";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseMeshDlgKey", "Choose Mesh File", ".obj, .off", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseMeshDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                meshPath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if(meshPath != ""){
            
            if (ImGui::Button("Cancel mesh")){
                meshPath = ""; 
            }

        }

        ImGui::Text("Selected Mesh File: %s", meshPath.c_str());

        // Chemin pour la texture
        if (ImGui::Button("Select Texture")) {
            IGFD::FileDialogConfig config;
              config.path = "../data/textures";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgTextureKey", "Choose File", ".png, .jpg, .bmp", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgTextureKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                texturePath = ImGuiFileDialog::Instance()->GetFilePathName();
            }

            ImGuiFileDialog::Instance()->Close();
        }
        if(texturePath != ""){

            if (ImGui::Button("Cancel texture")){
                texturePath = ""; 
            }

        }
      

        ImGui::Text("Selected Texture File: %s", texturePath.c_str());

        if (texturePath.empty()) {
            ImGui::Text("Color RGB (0-256)");
            static float colorWheel[3] = {1.0f, 1.0f, 1.0f};  // Valeurs normalisées de 0 à 1
            static bool colorPopupOpen = false;

            // Bouton pour ouvrir la roue de couleurs
            if (ImGui::Button("Choose a color")) {
                ImGui::OpenPopup("ColorPickerPopup");
            }

            // Pop-up de sélection de couleur
            if (ImGui::BeginPopup("ColorPickerPopup")) {
                ImGui::Text("Choose a color");
                ImGui::Separator();

                // Affiche la roue de couleur
                ImGui::ColorPicker3("Color", colorWheel);

                ImGui::Separator();
                if (ImGui::Button("OK", ImVec2(120, 0))) {
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
        if (ImGui::DragFloat3("Ambient", glm::value_ptr(ambient), 0.001f)) {
            material.setAmbient(ambient);
        }

        // Diffuse color
        glm::vec3 diffuse = material.getDiffuse();
        if (ImGui::DragFloat3("Diffuse", glm::value_ptr(diffuse), 0.001f)) {
            material.setDiffuse(diffuse);
        }

        // Specular color
        glm::vec3 specular = material.getSpecular();
        if (ImGui::DragFloat3("Specular", glm::value_ptr(specular), 0.001f)) {
            material.setSpecular(specular);
        }

        // Shininess
        float shininess = material.getShininess();
        if (ImGui::SliderFloat("Shininess", &shininess, 1.0f, 128.0f)) {
            material.setShininess(shininess);
        }


        // Ajustement du transform de base
        ImGui::Text("Position");
        glm::vec3 position = transform.getPosition();
        ImGui::DragFloat3("##Position", glm::value_ptr(position));

        ImGui::Text("Rotation");
        glm::vec3 rotation = transform.getRotation();
        ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation));

        ImGui::Text("Lock Scale");
        glm::vec3 scale = transform.getScale();
        ImGui::SameLine();
        ImGui::Checkbox("##LockScale", &scaleLocked_);

        if (scaleLocked_) {
            ImGui::Text("Scale");
            ImGui::DragFloat((std::string("##") + name + "Scale").c_str(), &scale.x, 0.1f, 0.0f, FLT_MAX);
            scale.y = scale.x;
            scale.z = scale.x;
        } else {
            ImGui::Text("Scale x, y, z");
            ImGui::DragFloat3((std::string("##") + name + "Scale").c_str(), glm::value_ptr(scale), 0.1f, 0.0f, FLT_MAX);
        }

        transform.setPosition(position);
        transform.setRotation(rotation);
        transform.setScale(scale);

        // Est ce que l'objet contient de la physique 
        ImGui::Text("Enable gravity");
        ImGui::SameLine();
        ImGui::Checkbox("##Physic", &physic);

        if(physic == true){
            ImGui::Text("Poids de l'objet");
            ImGui::SliderFloat("Poids", &poids, 0.0f, 10.0f);
        }
        
        // Création de l'objet
        if (ImGui::Button("Add Object")) {

            glActiveTexture(GL_TEXTURE0);
            GLuint textureID; 

            // Si pas de mesh en entrée alors erreur
            if (meshPath.empty()) {
                ImGui::OpenPopup("ErreurMesh");
            } else {
            
                GLuint textureID = texturePath.empty() ? 0 : loadTexture2DFromFilePath(texturePath);
                glUniform1i(glGetUniformLocation(programID, "gameObjectTexture"), 0);
                GameObject* newObject;
            
                newObject = new Mesh(name, meshPath.c_str(), textureID, texturePath.c_str(), programID);
                newObject->setMaterial(material); 
              
                
                if(textureID == 0){
                    newObject->setColor(color);  
                }
                newObject->setTransform(transform); 
                newObject->setInitalTransform(transform);
                SM->addObject(std::move(newObject->ptr));
                name[0] = '\0';
            }
          
        }

        if (ImGui::BeginPopupModal("ErreurMesh", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Erreur: Aucun mesh sélectionné.\nVeuillez choisir un fichier mesh avant de créer l'objet.");
            ImGui::Separator();

            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void updateInterface(float _deltaTime, GLFWwindow* _window)
	{

        if (ImGui::Begin("Interface")){
            if (ImGui::BeginTabBar("Tabs")) {
                camera->updateInterfaceCamera(_deltaTime); 
            }
            if (ImGui::BeginTabItem("Objects")) {
                for (auto& object : SM->getObjects()) {
                    std::string objectName = object->getName();
                    Mesh* meshObject = dynamic_cast<Mesh*>(object.get());
                    
                    
                    // Utiliser un bool pour suivre l'état du collapsable
                    static std::unordered_map<std::string, bool> collapsingState;
                    if (collapsingState.find(objectName) == collapsingState.end()) {
                        collapsingState[objectName] = true; // Définir l'état par défaut (ouvert)
                    }

                    // Gérer le collapsing avec un bool spécifique à chaque objet
                    bool isOpen = ImGui::CollapsingHeader(objectName.c_str(), 
                            ImGuiTreeNodeFlags_DefaultOpen * collapsingState[objectName]);

                    // Si l'état de l'élément change, on le met à jour
                    if (isOpen != collapsingState[objectName]) {
                        collapsingState[objectName] = isOpen;
                    }

                    if (isOpen) {
                        ImGui::Text("Object Name: %s", objectName.c_str());
                        // Récupère l'interface dans le gameObject pour la modification du transform
                        // object->updateInterfaceTransform(_deltaTime); 
                        meshObject->updateInterfaceTransform(_deltaTime); 
                    }
                }
                if (ImGui::CollapsingHeader("Add")) {
                    addGameObject(_deltaTime, _window);
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        ImGui::End();
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
        ImGuiIO& io = ImGui::GetIO();
        io.MouseWheel += static_cast<float>(yoffset); // Mettre à jour seulement pour le défilement vertical
    }

    void update(float _deltaTime, GLFWwindow* _window){
        updateInterface(_deltaTime, _window);
        bool isMenuFocused = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow | ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) ;

        // Si le menu est en focus et l'état n'est pas sauvegardé, on sauvegarde l'état
        if (isMenuFocused && !camera->m_stateSaved) {
            camera->saveState();          // Sauvegarde de l'état actuel
            camera->setInputMode(InputMode::Fixed); // Passage en mode fixe pour l'interface
        }
        
        // Si le menu perd le focus et que l'état a été sauvegardé, on restaure l'état
        else if (!isMenuFocused && camera->m_stateSaved) {
            camera->restoreState();       // Restauration de l'état initial
        }
    }

}; 
#endif