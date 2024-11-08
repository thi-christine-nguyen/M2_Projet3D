#ifndef PLAYER_HPP__
#define PLAYER_HPP__

#include "Objects/Sphere.hpp"

class Player : public Sphere{
private:
    float speed = 5.0f;
public:


    Player(std::string name, bool isPlayer, unsigned int res, unsigned int rad, int textureID = 0, const char *texturePath = "", GLuint programID = 0)
         : Sphere(name, isPlayer, res, rad, textureID, texturePath, programID) {}


    void handleInputs(GLFWwindow *window, float deltaTime){
        
        const float frictionCoefficient = 0.05f;
        const float velocityMax = 5.0f;
        const float rotateSpeed = 0.1f;
        const float accelerationMax = 10.0f;
        int isMovingX = 0;
        int isMovingY = 0;

        
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && grounded) {
            // this->setVelocity(glm::vec3(0.0f, 3.0f, 0.0f));
            this->velocity.y = 3.0f;
        }

        // Mouvement vers la droite
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && grounded) {
            isMovingX += isMovingX < 1 ? 1 : 0;
            if (this->acceleration.x < accelerationMax){
                this->acceleration.x += speed;
            }
            if (this->velocity.x < velocityMax){
                this->velocity.x += this->acceleration.x * deltaTime;
            }
            this->rotate(-rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
            // this->initTexture()
        }

        // Mouvement vers la gauche
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && grounded) {
            isMovingX -= isMovingX > -1 ? 1 : 0;
            if (this->acceleration.x > -1*accelerationMax){
                this->acceleration.x -= speed;
            }
            if (this->velocity.x > -1*velocityMax){
                this->velocity.x += this->acceleration.x * deltaTime;
            }
            this->rotate(rotateSpeed, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if(isMovingX == 0){
            this->acceleration.x = 0;
        }
        // Mouvement vers le haut
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && grounded) {
            isMovingY += isMovingY < 1 ? 1 : 0;
            if (this->acceleration.z < accelerationMax){
                this->acceleration.z += speed;
            }
            if (this->velocity.z < velocityMax){
                this->velocity.z += this->acceleration.z * deltaTime;
            }
            this->rotate(-rotateSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // Mouvement vers le bas
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && grounded) {
            isMovingY -= isMovingY > -1 ? 1 : 0;
            if (this->acceleration.z > -1*accelerationMax){
                this->acceleration.z -= speed;
            }
            if (this->velocity.z > -1*velocityMax){
                this->velocity.z += this->acceleration.z * deltaTime;
            }
            this->rotate(rotateSpeed, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if(isMovingY == 0){
            this->acceleration.z = 0;
        }

        // Appliquer le coefficient de friction pour réduire progressivement la vélocité dans les deux directions (X et Y)
        if (this->velocity.x > 0) {
            this->velocity.x -= frictionCoefficient;
            this->rotate(-rotateSpeed * this->velocity.x, glm::vec3(0.0f, 0.0f, 1.0f));
        } else if (this->velocity.x < 0) {
            this->velocity.x += frictionCoefficient;
            this->rotate(-rotateSpeed * this->velocity.x, glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (this->velocity.z > 0) {
            this->velocity.z -= frictionCoefficient;
            this->rotate(-rotateSpeed * this->velocity.z, glm::vec3(1.0f, 0.0f, 0.0f));
        } else if (this->velocity.z < 0) {
            this->velocity.z += frictionCoefficient;
            this->rotate(-rotateSpeed * this->velocity.z, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        
    }
};
#endif