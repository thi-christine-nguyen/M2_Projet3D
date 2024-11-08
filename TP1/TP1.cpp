#include "lib.hpp"
#include "Camera/Camera.hpp"
#include "PhysicManager.hpp"
#include "InputManager.hpp"
#include "Player.hpp"
#include "GameObject.hpp"
#include "Objects/Sphere.hpp"
#include "Objects/Plane.hpp"
#include "Objects/Cube.hpp"
#include "Interface.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


/*******************************************************************************/


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

    // Open a window and create its OpenGL context
    char title[50] = "Moteur de jeux - TP3 Graphe de scènes";
    window = glfwCreateWindow( 1024, 768, title, NULL, NULL);
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

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);

    // Création des managers
    // SceneManager *SM = new SceneManager();
    // PhysicManager *PM = new PhysicManager();
    // InputManager * IM = new InputManager();

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
    programID = LoadShaders( "vertex_shader.glsl", "fragment_shader.glsl" );
    Interface interface(programID); 
    glUseProgram(programID);

    //----------------------------------------- Init -----------------------------------------//

    // Création des différents GameObjects
    GameObject *pente = new Mesh("plan", "../data/meshes/plan2.obj", 1, "../data/textures/terrain.png", programID);
    
    // GameObject *pente =  new Plane("plan1", 1, 1, 1, "../data/textures/terrain.png", programID);
    // GameObject *pente = new Plane("landscape", 1, 10, 1, "../data/textures/terrain.png", programID);
    Player *player = new Player("player", true, 10, 1, 2, "../data/textures/ball.png", programID);
    // Player *player = new Player("player", "../data/meshes/sphereLow.obj", 3, "../data/textures/ball.png", programID);
    // // GameObject *cube = new Sphere("sphere", 5, 1, 3, "/mnt/c/Users/snsdt/Desktop/Projet_Moteur/data/textures/terrain.png", programID);
    // // Sphere *sphere = new Sphere("patate", "../data/meshes/sphere.obj", 3, "../data/textures/ball.jpg", programID); 
    // // Sphere *sphere = new Sphere("sphere", 20, 1, 3, "/mnt/c/Users/snsdt/Desktop/Projet_Moteur/data/textures/ball.png", programID);

    // landscape->getTransform()
    // pente->translate(glm::vec3(-1, -1, 0)); 
    pente->setInitalTransform(pente->getTransform()); 

    // landscape->translate(glm::vec3(0, 0, 0));
    // landscape->setInitalTransform(landscape->getTransform()); 
    
    
    // // Ajout des GameObjects au SceneManager
    interface.SM->addObject(std::move(pente->ptr));
    // interface.SM->addObject(std::move(landscape->ptr));

    // // Ajout des GameObjects au PhysicManager
    interface.PM->addObject(pente);
    // interface.PM->addObject(landscape);
   
    // Initialisation du player
    interface.setPlayer(player);  
    interface.getPlayer()->translate(glm::vec3(0.f, 5.f, 0.f));
    interface.getPlayer()->scale(glm::vec3(0.2));
    interface.getPlayer()->setWeight(0.6f);
    player->setInitalTransform(player->getTransform()); 

    
    interface.PM->addObject(interface.getPlayer());
    interface.SM->addObject(std::move(interface.getPlayer()->ptr));
    interface.SM->initGameObjectsTexture();

   

    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

    interface.initImgui(window);
    interface.camera.init();


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
            sprintf(title, "Moteur de jeux - TP4 Mouvements - (%.0f FPS)", FPS);
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
        if (interface.camera.getInputMode() == InputMode::Follow && interface.getPlayer() != nullptr)
            interface.camera.setCameraTarget(interface.getPlayer()->getTransform().getPosition());

        // interface.camera.setCameraTarget(basketBall->getTransform().getPosition());
        interface.update(deltaTime, window);
        interface.camera.update(deltaTime, window);
        interface.camera.sendToShader(programID); 
        // Input gérés par l'InputManager
        if(interface.getPlayer() != nullptr){
            interface.IM->processInput(window, interface.getPlayer(), deltaTime);
        }
        
        float updateTime = 1.0f/60.f;

        while (physicsClock >= updateTime) {

            interface.SM->update(deltaTime);

            // Check des collisions entre le plan et les gameObjects
            interface.PM->handleCollisions();

            // std::cout << "PM tick" << std::endl;
            physicsClock -= updateTime;
        }

       
        // Affichage de tous les élements de la scène via le SceneManager
        interface.SM->draw();
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

