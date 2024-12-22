#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "lib.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Interface.hpp"
#include "SceneManager.hpp"
#include "RegularGrid.hpp"

// /*******************************************************************************/
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
    // int window_width = static_cast<int>(mode->width * 0.8);
    // int window_height = static_cast<int>(mode->height * 1.0);

    
    int window_height = static_cast<int>(mode->height * 0.8);
    int window_width = static_cast<int>(mode->width * 0.5);

    
    float aspectRatio = static_cast<float>(window_width) / static_cast<float>(window_height);

    // Open a window and create its OpenGL context
    char title[50] = "Projet 3D - Voxelisation";
    GLFWwindow *window = glfwCreateWindow( window_width, window_height, title, NULL, NULL);
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
    
    Camera camera;
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwPollEvents();
    glfwSetCursorPos(window, window_width/2, window_height/2);
    glfwSetWindowUserPointer(window, &camera); // Associer le pointeur de l'objet Camera à la fenêtre

    glClearColor(0.8f, 0.8f, 0.8f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    // glDepthFunc(GL_LESS);
    // Cull triangles which normal is not towards the camera
    // glEnable(GL_CULL_FACE);

    // Create and compile our GLSL program from the shaders
    Shader shader = Shader("vertex_shader.glsl", "fragment_shader.glsl");
    Shader voxelShader = Shader("voxel_vertex_shader.glsl", "voxel_fragment_shader.glsl", "voxel_geometry_shader.glsl" );
    // Shader voxelShader = Shader("voxel_vertex_shader.glsl", "voxel_fragment_shader.glsl");
    SceneManager *SM = new SceneManager();


    Mesh *mesh = new Mesh("patate", "../data/meshes/sphere.off", glm::vec4(1.0f), shader);
    mesh->setInitalTransform(mesh->getTransform());
    SM->addObject(std::move(mesh->ptr));

   
    // RegularGrid grid = RegularGrid(mesh->getIndices(), mesh->getVertices(), 10);
    // grid.printGrid();

    // Mesh *mesh2 = new Mesh("mesh2", "../data/meshes/holy-cow.off", glm::vec4(0.0f, 1.f, 0.f, 1.0f), shader);
    // mesh2->setInitalTransform(mesh2->getTransform());
    // SM->addObject(std::move(mesh2->ptr));
    
    SM->initGameObjectsTexture();
    Interface interface(shader, SM, &camera); 

    //----------------------------------------- Init -----------------------------------------//
   // Timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    int nbFrames = 0;
    double totalDeltaTime = 0.;

    interface.initImgui(window); 

    do{
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        nbFrames++;
        totalDeltaTime += deltaTime;

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
        shader.use(); 

        interface.createFrame(); 
        interface.update(deltaTime, window); 

        camera.setupEditorLight(camera, shader.ID);
        camera.update(deltaTime, window); 
        camera.sendToShader(shader.ID, aspectRatio);

        SM->update(deltaTime);
        SM->draw(shader);


        voxelShader.use();
        camera.setupEditorLight(camera, voxelShader.ID);
        camera.sendToShader(voxelShader.ID, aspectRatio);
        SM->drawVoxel(voxelShader); 

        interface.renderFrame();

        // Swap buffers
        glfwSwapBuffers(window);

        // Traiter les événements de la fenêtre
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );

    interface.deleteFrame(); 


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


