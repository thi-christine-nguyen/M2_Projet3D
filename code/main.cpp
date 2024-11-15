#include "lib.hpp"
#include "Camera/Camera.hpp"
#include "PhysicManager.hpp"
#include "InputManager.hpp"
#include "GameObject.hpp"
#include "Interface.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// /*******************************************************************************/

// void GLFW_Scroll_Callback(GLFWwindow* window, double xoffset, double yoffset) {
//     // Mettez à jour les informations de défilement dans ImGui
//     ImGuiIO& io = ImGui::GetIO();
//     io.MouseWheelH += (float)xoffset;  // Défilement horizontal
//     io.MouseWheel += (float)yoffset;   // Défilement vertical
// }
// // Exemple pour enregistrer le callback de défilement dans la fonction de création de la fenêtre GLFW

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Récupérer la taille de l'écran
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Créer une fenêtre adaptative (par exemple 80% de la taille de l'écran)
    int window_width = static_cast<int>(mode->width * 0.5);
    int window_height = static_cast<int>(mode->height * 0.8);

    // Open a window and create its OpenGL context
    char title[50] = "Projet 3D - Voxelisation";
    window = glfwCreateWindow( window_width, window_height, title, NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    // glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // glfwSetScrollCallback(window, GLFW_Scroll_Callback);
 


    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, window_width/2, window_height/2);

    // Initialisation de la caméra
    glfwSetWindowUserPointer(window, &camera); // Associer le pointeur de l'objet Camera à la fenêtre

    // glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset) {
	// 	std::cout << "Scrolling " << xOffset << ", " << yOffset << std::endl;
    //     // Récupérer l'instance de Camera associée à la fenêtre
    //     Camera* camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    //     if (camera) {
    //         camera->scrollCallback(xOffset, yOffset); // Appeler la méthode de la classe Camera
    //     }
    // });

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // Création des managers
    SceneManager *SM = new SceneManager(programID);
    PhysicManager *PM = new PhysicManager();
    InputManager * IM = new InputManager();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint vertexbuffer;
    GLuint uvbuffer;
    GLuint elementbuffer;
    GLuint normalbuffer;
    GLuint tangentbuffer;
    GLuint bitangentbuffer;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("vertex_shader.glsl", "fragment_shader.glsl" );
    Interface interface(programID, SM, PM, IM, &camera); 
    glUseProgram(programID);

    //----------------------------------------- Init -----------------------------------------//

    // Création des différents GameObjects
    GameObject *cube = new Mesh("cube", "../data/meshes/cube.obj", 1, "../data/textures/grass.bmp", programID); 
    // GameObject *cube2 = new Mesh("cube2", "../data/meshes/cube.obj", 2, "../data/textures/terrain.png", programID); 


    
    cube->setInitalTransform(cube->getTransform()); 
    // cube2->setInitalTransform(cube2->getTransform()); 
    
    // Ajout des GameObjects au SceneManager
    SM->addObject(std::move(cube->ptr));
    // SM->addObject(std::move(cube2->ptr));

    SM->initGameObjectsTexture();

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    // GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    
    glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f); // Une position fixe pour la lumière
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);  // Couleur de la lumière (blanc)

    GLuint lightPosID = glGetUniformLocation(programID, "lightPos");
    GLuint lightColorID = glGetUniformLocation(programID, "lightColor");

    glUniform3fv(lightPosID, 1, &lightPos[0]);
    glUniform3fv(lightColorID, 1, &lightColor[0]);


  
  


    // Init la fenêtre d'interface ImGUI
    interface.initImgui(window);

    int t = 0; 

    do{
        // Measure speed
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        nbFrames++;
        totalDeltaTime += deltaTime;
        physicsClock += deltaTime;

        if (totalDeltaTime >= 1) { // On mets à jour les FPS toutes les secondes
            float FPS = nbFrames / totalDeltaTime;
            sprintf(title, "Projet 3D - Voxélisation - (%.0f FPS)", FPS);
            glfwSetWindowTitle(window, title);
            nbFrames = 0;
            totalDeltaTime = 0.;
        }

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Optimisation du rendu en cachant les éléments non visibles
        glEnable(GL_DEPTH_TEST);

        // Use our shader
        glUseProgram(programID);

        //Imgui 
        interface.createFrame();

        // camera.setCameraTarget(basketBall->getTransform().getPosition());
        interface.update(deltaTime, window);
        camera.update(deltaTime, window);
        camera.sendToShader(programID); 
        // Input gérés par l'InputManager
        IM->processInput(window, deltaTime);
        
        float updateTime = 1.0f/60.f;

        while (physicsClock >= updateTime) {
            SM->update(deltaTime);
            // Check des collisions entre le plan et les gameObjects
            PM->handleCollisions();

            // std::cout << "PM tick" << std::endl;
            physicsClock -= updateTime;
        }

        // Affichage de tous les élements de la scène via le SceneManager
        SM->draw();
        interface.renderFrame(); 

        // Swap buffers
        glfwSwapBuffers(window);

        // Traiter les événements de la fenêtre
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    

    interface.deleteFrame();


    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(programID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

