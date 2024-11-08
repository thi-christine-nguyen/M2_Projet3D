#ifndef CAMERA_HPP__
#define CAMERA_HPP__

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

class Camera {
private:
    glm::vec3 position; 
    glm::vec3 front; 
    glm::vec3 up; 
    glm::vec3 right;
    glm::vec3 target;

    glm::vec3 worldUp; // Vecteur vers le haut du repère monde

    float yaw;
    float pitch;
    float zoom = 45.0f;
    float fov = 45.f; // Angle de champ de vue (field of view) en degrés
    float sensitivity = 0.3f;
    float cameraArmLenght = 10.f;

    float speed = 0.05f;
    bool isOrbiting; // Indicateur pour savoir si la caméra est en mode orbite
    float orbitSpeed; // Vitesse de rotation en mode orbite

public:
    /* ------------------------- CONSTRUCTOR -------------------------*/
    Camera(glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float initialYaw = -90.0f, float initialPitch = 0.0f, float initialFov = 45.0f) 
        : position(initialPosition), front(glm::vec3(0.0f, 0.0f, -1.0f)), up(worldUp), right(-glm::normalize(glm::cross(up, front))),
          speed(0.05f), isOrbiting(false), orbitSpeed(0.001f), worldUp(worldUp), fov(initialFov) {
            updateCameraVectors();
          }

    /* ------------------------- GETTERS/SETTERS -------------------------*/
    // Accesseurs pour la matrice de vue
    glm::mat4 getViewMatrix() { return glm::lookAt(position, isOrbiting ? front : position + front, up); }

    // Accesseurs pour la position de la caméra
    glm::vec3 getPosition() const { return position; }

    // Accesseurs pour la direction de la caméra
    glm::vec3 getFront() const { return front; }

    // Accesseurs pour le vecteur vers le haut de la caméra
    glm::vec3 getUp() const { return up; }

    // Accesseurs pour le vecteur vers la droite de la caméra
    glm::vec3 getRight() const { return right; }

    // Accesseurs pour la vitesse de déplacement de la caméra
    float getSpeed() const { return speed; }

    // Accesseurs pour l'indicateur d'orbite de la caméra
    bool getIsOrbiting() const { return isOrbiting; }

    // Accesseurs pour la vitesse d'orbite de la caméra
    float getOrbitSpeed() const { return orbitSpeed; }

    // Accesseurs pour l'angle de champ de vue de la caméra
    float getFov() const { return fov; }

    // Accesseurs pour la sensibilité de la caméra
    float getSensitivity() const { return sensitivity; }

    // Mutateur pour la position de la caméra
    void setPosition(const glm::vec3& newPos) { position = newPos; }

    // Mutateur pour la target de la caméra
    void setFront(const glm::vec3& newFront) { front = newFront; }

    // Mutateur pour la vitesse de déplacement de la caméra
    void setSpeed(float newSpeed) { speed = newSpeed; }

    // Mutateur pour l'indicateur d'orbite de la caméra
    void setIsOrbiting(bool newIsOrbiting) { isOrbiting = newIsOrbiting; }

    // Mutateur pour la vitesse d'orbite de la caméra
    void setOrbitSpeed(float newOrbitSpeed) { orbitSpeed = newOrbitSpeed; }

    // Mutateur pour l'angle de champ de vue de la caméra
    void setFov(float newFov) { fov = newFov; }

    // Mutateur pour le zoom de la caméra
    void setZoom(float newZoom) { zoom = newZoom; }

    /* ------------------------- INPUTS -------------------------*/

    void processKeyboard(GLFWwindow *window, float deltaTime) {
        float velocity = speed * deltaTime;

        //----------------------------------- Camera Movements -----------------------------------//

        // UP
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !isOrbiting) {
            position += up * speed;
        }

        // DOWN
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !isOrbiting) {
            position -= up * speed;
        }

        // RIGHT
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !isOrbiting) {
            right = -glm::normalize(glm::cross(up, front));
            position += right * speed;
        }

        // LEFT
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !isOrbiting) {
            right = -glm::normalize(glm::cross(up, front));
            position -= right * speed;
        }

        // ZOOM IN
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !isOrbiting) {
            position += front * speed;
        }

        // ZOOM OUT
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !isOrbiting) {
            position -= front * speed;
        }

        //------------------------------------- Camera Orbit VS libre -------------------------------------//

        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
            if (!isOrbiting) {
                isOrbiting = true;
            }
        }

        if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
        {
            if (isOrbiting) {
                isOrbiting = false;
            }
        }

        if (isOrbiting) {
            // float radius = glm::distance(position, front);
            float radius = cameraArmLenght;
            float horizontalAngle = atan2(position.z - front.z, position.x - front.x);
            float verticalAngle = atan2(position.y - front.y + 5, radius / 1.1);

            horizontalAngle -= orbitSpeed;

            float newCameraPosX = front.x + radius * cos(verticalAngle) * cos(horizontalAngle);
            float newCameraPosZ = front.z + radius * cos(verticalAngle) * sin(horizontalAngle);
            float newCameraPosY = front.y + radius * sin(verticalAngle);
            position = glm::vec3(newCameraPosX, newCameraPosY, newCameraPosZ);
            front = glm::normalize(glm::vec3(0.) - position);
        }

        //---------------------------------- Camera Orbit Speed ----------------------------------//
        
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            orbitSpeed += orbitSpeed < 0.25 ? .005 : 0.;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            orbitSpeed -= orbitSpeed > -0.25 ? .005 : 0.;
        }
    }

    // Déplacement de la caméra immersive (NON FONCTIONNEL)
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        yaw += xoffset * sensitivity;
        pitch += yoffset * sensitivity;

        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

    // Fonction qui modifie la caméra en fonction du scroll souris
    // De base j'avais mis le zoom mais marche moyennement donc controle vitesse caméra
    void processMouseScroll(float yoffset)
    {
        float deltaSpeed = yoffset/100;
        float deltaOrbitSpeed = yoffset/200;
        if (speed + deltaSpeed >= 0.01f && speed + deltaSpeed <= 0.5f)
            speed += deltaSpeed;
        // std::cout << "speed: " << speed << " yoffset: " << deltaSpeed << std::endl;
        if (orbitSpeed + deltaOrbitSpeed >= -0.25f && orbitSpeed + deltaOrbitSpeed <= 0.25f)
            orbitSpeed += deltaOrbitSpeed;
        // std::cout << "orbitSpeed: " << orbitSpeed << " yoffset: " << deltaOrbitSpeed << std::endl;
    }

    /* ------------------------- OTHER METHODS -------------------------*/

    void sendToShader(GLuint programID) {
        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
        glm::mat4 mat_v = getViewMatrix();
        GLuint id_v = glGetUniformLocation(programID, "view_mat");
        glUniformMatrix4fv(id_v, 1, false, &mat_v[0][0]);

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 mat_p = glm::perspective(glm::radians(fov), (float)4 / (float)3, 0.1f, 100.0f);
        GLuint id_p = glGetUniformLocation(programID, "project_mat");
        glUniformMatrix4fv(id_p, 1, false, &mat_p[0][0]);
    }

    // Fonction permettant de calculer les vecteurs camera selon le yaw et le pitch
    // (Non fonctionnel pour l'instant on verra plus tard)
    void updateCameraVectors() {
        glm::vec3 newFront;
        newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newFront.y = sin(glm::radians(pitch));
        newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(newFront);

        right = glm::normalize(glm::cross(front, worldUp));
        up    = glm::normalize(glm::cross(right, front));
    }

    void resetCamera() {
        float radius = cameraArmLenght;
        float horizontalAngle = atan2(position.z - front.z, position.x - front.x);
        float verticalAngle = atan2(position.y - front.y + 5, radius / 1.1);

        float newCameraPosX = front.x + radius * cos(verticalAngle) * cos(horizontalAngle);
        float newCameraPosZ = front.z + radius * cos(verticalAngle) * sin(horizontalAngle);
        float newCameraPosY = front.y + radius * sin(verticalAngle);
        position = glm::vec3(newCameraPosX, newCameraPosY, newCameraPosZ);
        front = glm::normalize(glm::vec3(0.) - position);
    }
};

#endif