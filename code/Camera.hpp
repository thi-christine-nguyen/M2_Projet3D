#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <array>
#include <memory>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


enum class InputMode {
    Fixed, // Mode fixe
    Free   // Mode libre
};

class Camera
{
    
private:
    glm::vec3 m_target;         // Target point the camera orbits around
    glm::vec3 m_position;       // Camera position
    glm::vec3 m_upDirection;    // Camera's up direction
    float m_distance;           // Distance from target
    float m_minDistance;        // Minimum zoom distance
    float m_maxDistance;        // Maximum zoom distance
    float m_rotationSpeed;      // Speed of rotation
    float m_zoomSpeed;          // Speed of zoom
    float m_fovDegree;          // Field of view for perspective projection
    double m_lastMouseX;        // Last X position of the mouse
    double m_lastMouseY;        // Last Y position of the mouse
    float m_movementSpeed; 
    float m_translationSpeed; 
    float m_minRotationSpeed; 
    float m_maxRotationSpeed; 
    double m_scrollOffset;      // Scroll offset for zooming
    glm::quat m_orientation;    // Camera's orientation as a quaternion
    glm::mat4 m_viewMatrix;     // Cached view matrix

    bool m_stateSaved = false;
    glm::vec3 m_savedPosition;
    glm::quat m_savedOrientation;
    InputMode m_currentMode = InputMode::Free;

public:
    Camera();

    void update(float deltaTime, GLFWwindow* window);
    void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
    void setupEditorLight(GLuint shaderID);
    void sendToShader(GLuint programID, float aspectRatio) const;

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void saveState();
    bool getSavedState();
    void restoreState();
    void setInputMode(InputMode mode);

};

#endif