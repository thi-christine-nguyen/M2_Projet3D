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
#include "PhysicManager.hpp"
#include "InputManager.hpp"
#include "Objects/Sphere.hpp"
#include "Objects/Plane.hpp"
#include "Objects/Mesh.hpp"
#include "Objects/Landscape.hpp"
#include "Player.hpp"



class Interface
{
private : 
    GLuint programID;

public : 

    Camera camera; 
    SceneManager *SM;
    PhysicManager *PM; 
    InputManager *IM; 
    

    Interface(GLuint _programID, SceneManager *_SM, PhysicManager *_PM, InputManager *_IM) :
    programID(_programID),
    SM(_SM),
    PM(_PM),
    IM(_IM) {}

    void initImgui(GLFWwindow *window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    void createFrame(){
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
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
        static char name[128] = "";
        static GameObjectType selectedType = SPHERE; 
        static int resolution;
        static int size;
        static std::string meshPath;
        static std::string texturePath;
        static bool playable = false; 
        static bool physic = false; 
        static float poids = 0; 
        static Transform transform; 
        static bool scaleLocked_ = false; 

        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

        // Boutons de radio pour choisir le type d'objet à créer
        ImGui::Text("Select Object Type:");
        ImGui::RadioButton("Sphere", reinterpret_cast<int*>(&selectedType), SPHERE);
        ImGui::RadioButton("Mesh", reinterpret_cast<int*>(&selectedType), MESH);
        ImGui::RadioButton("Plane", reinterpret_cast<int*>(&selectedType), PLANE);

        // Chemin pour le mesh
        if (ImGui::Button("Mesh Path")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseMeshDlgKey", "Choose Mesh File", ".obj", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseMeshDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                meshPath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if (ImGui::Button("Annuler le mesh")){
            meshPath = ""; 
        };

        if(meshPath == ""){
            ImGui::InputInt("Resolution", &resolution);
            ImGui::InputInt("Size/Radius", &size);
        }

        ImGui::Text("Selected Mesh File: %s", meshPath.c_str());

        // Chemin pour la texture
        if (ImGui::Button("Texture Path")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgTextureKey", "Choose File", ".png, .jpg, .bmp", config);
        }
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgTextureKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
                texturePath = ImGuiFileDialog::Instance()->GetFilePathName();
            }

            ImGuiFileDialog::Instance()->Close();
        }

        ImGui::Text("Selected Texture File: %s", texturePath.c_str());

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
            GLuint textureID = loadTexture2DFromFilePath(texturePath); 
            glUniform1i(glGetUniformLocation(programID, "gameObjectTexture"), 0);

            GameObject* newObject;
            
            switch (selectedType) {
                case SPHERE :
                    newObject = new Sphere(name, resolution, size, textureID, texturePath.c_str(), programID);
                    break;

                case MESH:
                    newObject = new Mesh(name, meshPath.c_str(), textureID, texturePath.c_str(), programID);
                    break;

                case PLANE:
                    newObject = new Plane(name, resolution, size, textureID, texturePath.c_str(), programID);
                    break;
            }

            if(selectedType != SPHERE){
                newObject->setTransform(transform); 
                newObject->setInitalTransform(transform); 
                if(physic == true){
                    newObject->setWeight(poids); 
                    PM->addObject(newObject); 
                    
                }

                SM->addObject(std::move(newObject->ptr));
            }

            name[0] = '\0';
            resolution = 0; 
            size = 0; 
            selectedType = SPHERE;
        }
    }

    void updateInterface(float _deltaTime, GLFWwindow* _window)
	{
        if (ImGui::Begin("Interface")){
            if (ImGui::BeginTabBar("Tabs")) {
                camera.updateInterfaceCamera(_deltaTime); 
            }
            if (ImGui::BeginTabItem("Objects"))
            {
                for (const auto& object : SM->getObjects()) {
                    std::string objectName = object->getName();
                    if (ImGui::CollapsingHeader(objectName.c_str())) {
                        ImGui::Text("Object Name: %s", objectName.c_str());
                        // Récupère l'interface dans le gameObject pour la modification du transform
                        object->updateInterfaceTransform(_deltaTime); 
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

    void update(float _deltaTime, GLFWwindow* _window){
        updateInterface(_deltaTime, _window);
        bool isMenuFocused = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow);

        // Si le menu est en focus et l'état n'est pas sauvegardé, on sauvegarde l'état
        if (isMenuFocused && !camera.m_stateSaved) {
            camera.saveState();          // Sauvegarde de l'état actuel
            camera.setInputMode(InputMode::Fixed); // Passage en mode fixe pour l'interface
        }
        // Si le menu perd le focus et que l'état a été sauvegardé, on restaure l'état
        else if (!isMenuFocused && camera.m_stateSaved) {
            camera.restoreState();       // Restauration de l'état initial
        }
    }



}; 
#endif