#include "Camera.hpp"

Camera::Camera()
    : m_target(0.0f, 0.0f, 0.0f),
      m_position(0.0f, 0.0f, 5.0f),
      m_upDirection(0.0f, 1.0f, 0.0f),
      m_distance(5.0f),
      m_minDistance(1.0f),
      m_maxDistance(20.0f),
      m_rotationSpeed(0.1f),
      m_zoomSpeed(1.0f),
      m_fovDegree(45.0f),
      m_lastMouseX(0.0),
      m_lastMouseY(0.0),
      m_movementSpeed(5.0f),
      m_translationSpeed(0.5f),
      m_scrollOffset(0.0),
      m_minRotationSpeed(0.1f),
      m_maxRotationSpeed(5.0f),
      m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) // Identity quaternion
{
}

void Camera::saveState()
{
    if (!m_stateSaved)
    {
        m_savedPosition = m_position;      // Sauvegarde la position
        m_savedOrientation = m_orientation; // Sauvegarde l'orientation
        m_stateSaved = true;
    }
}

void Camera::restoreState()
{
    if (m_stateSaved)
    {
        m_position = m_savedPosition;      // Restaure la position
        m_orientation = m_savedOrientation; // Restaure l'orientation
        m_stateSaved = false;
        m_currentMode = InputMode::Free; 
    }
}

bool Camera::getSavedState(){
    return m_stateSaved; 
}

void Camera::setInputMode(InputMode mode)
{
    m_currentMode = mode;
}

void Camera::update(float deltaTime, GLFWwindow* window)
{
    // Mouse input
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    
    // Keyboard input for free movement
    glm::vec3 forward = glm::rotate(m_orientation, glm::vec3(0.0f, 0.0f, -1.0f));
    glm::vec3 right = glm::rotate(m_orientation, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 up = glm::rotate(m_orientation, glm::vec3(0.0f, 1.0f, 0.0f));

    if (m_currentMode == InputMode::Fixed) return;


    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        double deltaX = mouseX - m_lastMouseX;
        double deltaY = mouseY - m_lastMouseY;

        // Update orientation using quaternions
        glm::quat yawQuat = glm::angleAxis(static_cast<float>(-deltaX) * m_rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchQuat = glm::angleAxis(static_cast<float>(-deltaY) * m_rotationSpeed * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
        m_orientation = glm::normalize(yawQuat * m_orientation * pitchQuat);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_position += forward * m_movementSpeed * deltaTime; // Forward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_position -= forward * m_movementSpeed * deltaTime; // Backward
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_position -= right * m_movementSpeed * deltaTime;   // Left
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_position += right * m_movementSpeed * deltaTime;   // Right
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) m_position += up * m_movementSpeed * deltaTime;  // Up
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) m_position -= up * m_movementSpeed * deltaTime; // Down
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double deltaX = mouseX - m_lastMouseX;
        double deltaY = mouseY - m_lastMouseY;

        // Update orientation using quaternions
        glm::quat yawQuat = glm::angleAxis(static_cast<float>(-deltaX) * m_rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        m_orientation = glm::normalize(yawQuat * m_orientation);

        // Translation en profondeur basée sur le mouvement vertical de la souris
        glm::vec3 forward = glm::rotate(m_orientation, glm::vec3(0.0f, 0.0f, -1.0f)); // Direction "avant"
        m_position += forward * static_cast<float>(-deltaY) * m_translationSpeed * deltaTime;

            
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) m_position += forward * m_movementSpeed * deltaTime; // Forward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) m_position -= forward * m_movementSpeed * deltaTime; // Backward
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) m_position -= right * m_movementSpeed * deltaTime;   // Left
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) m_position += right * m_movementSpeed * deltaTime;   // Right
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) m_position += up * m_movementSpeed * deltaTime;  // Up
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) m_position -= up * m_movementSpeed * deltaTime; // Down
    }

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    // Update the up direction
    m_upDirection = glm::rotate(m_orientation, glm::vec3(0.0f, 1.0f, 0.0f));

    // Update the view matrix
    m_viewMatrix = glm::lookAt(m_position, m_position + forward, m_upDirection);
}


void Camera::scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    m_scrollOffset = yOffset; // Update scroll offset
}



void Camera::sendToShader(GLuint programID, float aspectRatio) const
{
    // View matrix
    GLuint viewLoc = glGetUniformLocation(programID, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);

    // Projection matrix
    glm::mat4 projectionMatrix = getProjectionMatrix(aspectRatio);
    GLuint projLoc = glGetUniformLocation(programID, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projectionMatrix[0][0]);
}

glm::mat4 Camera::getViewMatrix() const
{
    return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fovDegree), aspectRatio, 0.1f, 100.0f);
}
