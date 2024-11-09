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
#include "Mesh.hpp"



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

        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

        // Chemin pour le mesh
        if (ImGui::Button("Mesh Path")) {
            IGFD::FileDialogConfig config;
            config.path = ".";
            ImGuiFileDialog::Instance()->OpenDialog("ChooseMeshDlgKey", "Choose Mesh File", ".obj, .off", config);
        }

        if (ImGuiFileDialog::Instance()->Display("ChooseMeshDlgKey")) {
            if (ImGuiFileDialog::Instance()->IsOk()) {
                meshPath = ImGuiFileDialog::Instance()->GetFilePathName();
            }
            ImGuiFileDialog::Instance()->Close();
        }

        if(meshPath != ""){
            
            if (ImGui::Button("Annuler le mesh")){
                meshPath = ""; 
            }

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
        if(texturePath != ""){

            if (ImGui::Button("Annuler le texture")){
                texturePath = ""; 
            }

        }
      

        ImGui::Text("Selected Texture File: %s", texturePath.c_str());

        if (texturePath == "") {
            ImGui::Text("Color RGB (0-256)");
            static int colorRGB[3] = {255, 255, 255};

            // Champs de saisie pour chaque composant RGB, valeurs de 0 à 256
            ImGui::InputInt("Red", &colorRGB[0], 1, 10, ImGuiInputTextFlags_CharsDecimal);
            ImGui::InputInt("Green", &colorRGB[1], 1, 10, ImGuiInputTextFlags_CharsDecimal);
            ImGui::InputInt("Blue", &colorRGB[2], 1, 10, ImGuiInputTextFlags_CharsDecimal);

            // S'assurer que les valeurs sont dans la plage [0, 256]
            colorRGB[0] = glm::clamp(colorRGB[0], 0, 256);
            colorRGB[1] = glm::clamp(colorRGB[1], 0, 256);
            colorRGB[2] = glm::clamp(colorRGB[2], 0, 256);

            // Affichage de la couleur résultante avec les valeurs RGB
            ImGui::Text("Selected ColorRGB: R%d G%d B%d", colorRGB[0], colorRGB[1], colorRGB[2]);

            // Si vous devez les utiliser dans un shader ou pour des calculs, vous pouvez normaliser
            // Divisez chaque composant par 255.0f pour obtenir une valeur dans la plage [0, 1]
            float normalizedColor[3] = {
                color[0] = colorRGB[0] / 255.0f,
                color[1] = colorRGB[1] / 255.0f,
                color[2] = colorRGB[2] / 255.0f
            };
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
            
            if(texturePath == ""){
                textureID = 0; 
                
            }else{
                textureID = loadTexture2DFromFilePath(texturePath); 
            }
            glUniform1i(glGetUniformLocation(programID, "gameObjectTexture"), 0);

            GameObject* newObject;
          
            newObject = new Mesh(name, meshPath.c_str(), textureID, texturePath.c_str(), programID);
            if(textureID == 0){
                newObject->setColor(color);  
            }
             
        
            newObject->setTransform(transform); 
            newObject->setInitalTransform(transform); 
            if(physic == true){
                newObject->setWeight(poids); 
                PM->addObject(newObject); 
            }

            SM->addObject(std::move(newObject->ptr));
            name[0] = '\0';
          
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