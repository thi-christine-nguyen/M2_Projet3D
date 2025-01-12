#include "Interface.hpp"

void Interface::initImgui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Interface::createFrame() {
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



void Interface::renderFrame(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Deletes all ImGUI instances
void Interface::deleteFrame(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Interface::addGameObject(float _deltaTime, GLFWwindow* _window){
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
        ImGuiFileDialog::Instance()->OpenDialog("ChooseMeshDlgKey", "Choose Mesh File", ".off", config);
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
    
    // Création de l'objet
    if (ImGui::Button("Add Object")) {

        glActiveTexture(GL_TEXTURE0);
        GLuint textureID; 

        // Si pas de mesh en entrée alors erreur
        if (meshPath.empty()) {
            ImGui::OpenPopup("ErreurMesh");
        } else {
        
            GLuint textureID = texturePath.empty() ? 0 : loadTexture2DFromFilePath(texturePath);
            glUniform1i(glGetUniformLocation(shader.ID, "gameObjectTexture"), 0);
            GameObject* newObject;
        
            newObject = new Mesh(name, meshPath.c_str(), textureID, texturePath.c_str(), shader);
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

void voxelInterface(Mesh* mesh){
    ImGui::Separator();
    ImGui::Text("Type de Grille");
    const char* gridTypeNames[] = { "Regular Grid", "Adaptative Grid" };
    int currentGridType = static_cast<int>(mesh->getGridType());

    if (ImGui::Combo(("##" + std::to_string(mesh->getId()) + "GridType").c_str(), &currentGridType, gridTypeNames, IM_ARRAYSIZE(gridTypeNames))) {
        mesh->setGridType(static_cast<GridType>(currentGridType));
    }

    ImGui::Separator();

    ImGui::Text("Resolution de voxelisation");
    ImGui::SliderInt(("##" + std::to_string(mesh->getId()) + "VoxelResolution").c_str(), &mesh->getVoxelResolution(), 2, 30);

    // Liste des méthodes de voxélisation
    static int selectedMethod = 0; // Indice de la méthode sélectionnée
    
    ImGui::Text("Méthodes de voxélisation");
    if(mesh->getGridType() == GridType::Regular){
        const char* voxelMethods[] = { "Optimized", "Simple", "Surface" };
        ImGui::Combo(("##" + std::to_string(mesh->getId()) + "VoxelMethod").c_str(), &selectedMethod, voxelMethods, IM_ARRAYSIZE(voxelMethods));

    }

    // Bouton pour voxeliser
    if (ImGui::Button(("Voxeliser ##" + std::to_string(mesh->getId())).c_str())) {
        if (mesh->getVoxelResolution() > 0) {
            VoxelizationMethod method = (selectedMethod == 0) ? VoxelizationMethod::Optimized :
                                         (selectedMethod == 1) ? VoxelizationMethod::Simple :
                                         VoxelizationMethod::Surface;

            if (mesh->getGridType() == GridType::Regular) {
                mesh->setGrid(std::make_unique<RegularGrid>(mesh->getIndices(), mesh->getVertices(), mesh->getVoxelResolution(), method));
            } else {
                mesh->setGrid(std::make_unique<AdaptativeGrid>(mesh->getIndices(), mesh->getVertices(), mesh->getVoxelResolution(), method));
            }

            // grid->marchingCubeInterface(); 
            mesh->setShowVoxel(true);
            mesh->setGridInitialized(true); 
        }
    }

    if (mesh->isGridInitialized()){
        
        ImGui::Text("Color RGB (0-256)");
        static float colorWheel[3] = {1.0f, 1.0f, 1.0f};  // Valeurs normalisées de 0 à 1
        static bool colorPopupOpen = false;

        // Bouton pour ouvrir la roue de couleurs
        if (ImGui::Button(("Choose a color ## voxel" + std::to_string(mesh->getId())).c_str())) {
            ImGui::OpenPopup(("ColorPickerPopup ## voxel" + std::to_string(mesh->getId())).c_str());
        }

        // Pop-up de sélection de couleur
        if (ImGui::BeginPopup(("ColorPickerPopup ## voxel" + std::to_string(mesh->getId())).c_str())) {
            ImGui::Text("Choose a color");
            ImGui::Separator();

            // Affiche la roue de couleur
            ImGui::ColorPicker3(("Color ## voxel" + std::to_string(mesh->getId())).c_str(), colorWheel);

            ImGui::Separator();
            if (ImGui::Button(("OK ## voxel" + std::to_string(mesh->getId())).c_str(), ImVec2(120, 0))) {
                glm::vec3 selectedColor(colorWheel[0], colorWheel[1], colorWheel[2]);
                mesh->getGrid()->setColor(selectedColor);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::Checkbox(("Afficher le Mesh ##" + std::to_string(mesh->getId())).c_str(), &mesh->isShowMesh());
    ImGui::Checkbox(("Afficher le Mesh Wireframe ##" + std::to_string(mesh->getId())).c_str(), &mesh->getIsWireframe());

    ImGui::Checkbox(("Afficher en Voxel ##" + std::to_string(mesh->getId())).c_str(), &mesh->isShowVoxel());
    ImGui::Checkbox(("Afficher Voxel en Wireframe ##" + std::to_string(mesh->getId())).c_str(), &mesh->getIsWireframeVoxel());
}


void marchingCubeInterface(Mesh* mesh){
    ImGui::Separator();
    ImGui::Text("Marching Cubes and OFF Export");
    static bool isMarchingCubeExecuted = false;
    static std::vector<unsigned short> indices;
    static std::vector<glm::vec3> vertices;

    // Bouton pour exécuter l'algorithme Marching Cubes
    if (ImGui::Button("Run Marching Cubes")) {
        indices.clear(); 
        vertices.clear(); 

        // Vérifie si la grille est initialisée avant d'exécuter Marching Cubes
        if (mesh->isGridInitialized()) {
            mesh->getGrid()->marchingCube(indices, vertices);

            std::cout << "Marching Cubes executed successfully!" << std::endl;
            isMarchingCubeExecuted = true;
        } else {
            ImGui::Text("Grid not initialized or missing!");
        }
    }
    if (isMarchingCubeExecuted) {
        static char filename[128] = "../data/meshes/output.off";
        ImGui::InputText("Filename", filename, IM_ARRAYSIZE(filename));

        ImGui::SameLine(); 
        if (ImGui::Button("Export OFF File")) {
            std::string file = filename;
            mesh->getGrid()->createOffFile(indices, vertices, file);
        }
    }
}


void Interface::updateInterface(float _deltaTime, GLFWwindow* _window)
{

    if (ImGui::Begin("Interface")){
        if (ImGui::BeginTabBar("Tabs")) {
        //     camera->updateInterfaceCamera(_deltaTime); 
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
                    meshObject->updateInterfaceTransform(_deltaTime); 
                    voxelInterface(meshObject); 
                    if(meshObject->isGridInitialized()){marchingCubeInterface(meshObject);}
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

void Interface::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += static_cast<float>(yoffset); // Mettre à jour seulement pour le défilement vertical
}

void Interface::update(float _deltaTime, GLFWwindow* _window){
    updateInterface(_deltaTime, _window);
    bool isMenuFocused = ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered(ImGuiFocusedFlags_AnyWindow | ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) ;

    // Si le menu est en focus et l'état n'est pas sauvegardé, on sauvegarde l'état
    if (isMenuFocused && !camera->getSavedState()) {
        camera->saveState();          // Sauvegarde de l'état actuel
        camera->setInputMode(InputMode::Fixed); // Passage en mode fixe pour l'interface
    }
    
    // Si le menu perd le focus et que l'état a été sauvegardé, on restaure l'état
    else if (!isMenuFocused && camera->getSavedState()) {
        camera->restoreState();       // Restauration de l'état initial
    }
}
