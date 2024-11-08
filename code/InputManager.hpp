#ifndef INPUT_MANAGER_HPP__
#define INPUT_MANAGER_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "variables.hpp"
#include "Player.hpp"

void targetMesh(std::vector<glm::vec3> &indexed_vertices );

//----------------------------ZOOM et MOUSETRACKING ----------------------------------//
double xpos, ypos;               // Position actuelle de la souris
double lastX = 0.0, lastY = 0.0; // Position précédente de la souris
bool firstMouse = true;          // Indicateur pour la première utilisation de la souris
float rotatevalue = 0.f;

// Fonction de rappel pour la position de la souris
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    // Si c'est la première fois que la souris est utilisée, initialiser les valeurs de position précédente
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculer le déplacement de la souris par rapport à la dernière boucle
    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos; // La distance y est inversée (coordonnées écran)

    // Mettre à jour la position de la caméra en fonction du déplacement de la souris
    // if (!camera.getIsOrbiting())
    //     camera.processMouseMovement(xoffset * camera.getSensitivity(), yoffset * camera.getSensitivity());
    // Sauvegarder la position précédente de la souris
    lastX = xpos;
    lastY = ypos;
}

// Définition de la fonction de rappel pour le scroll de la souris
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    // camera.processMouseScroll(yoffset);
}

class InputManager {

public:
    // CONSTRUCTEUR
    InputManager() {}

    // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
    // ---------------------------------------------------------------------------------------------------------
    void processInput(GLFWwindow *window,  float deltaTime)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //----------------------------------- Camera Movements -----------------------------------//
        // camera.processKeyboard(window, deltaTime);
        updateMouse(window);

        //----------------------------------- model Focus -----------------------------------//
        // if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !camera.getIsOrbiting()) {
        //     // camera.resetCamera();
        // }

        //----------------------------------- Display mode -----------------------------------//

        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
            glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        }
        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
            glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        }
    }

    void updateMouse(GLFWwindow *window) {
        glfwGetCursorPos(window, &xpos, &ypos);
        // Définir la fonction de rappel pour la position de la souris
        // glfwSetCursorPosCallback(window, mouse_callback);
        // Ajout de la fonction de rappel à la fenêtre GLFW
        glfwSetScrollCallback(window, scroll_callback);
    }

};

#endif