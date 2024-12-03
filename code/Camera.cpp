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
      m_scrollOffset(0.0),
      m_orientation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)) // Identity quaternion
{
}

void Camera::update(float deltaTime, GLFWwindow* window)
{
    // Mouse input
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        double deltaX = mouseX - m_lastMouseX;
        double deltaY = mouseY - m_lastMouseY;

        // Update orientation using quaternions
        glm::quat yawQuat = glm::angleAxis(static_cast<float>(deltaX) * m_rotationSpeed * deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::quat pitchQuat = glm::angleAxis(static_cast<float>(-deltaY) * m_rotationSpeed * deltaTime, glm::vec3(1.0f, 0.0f, 0.0f));
        m_orientation = glm::normalize(yawQuat * m_orientation * pitchQuat);
    }

    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    // Handle zoom with scroll
    if (m_scrollOffset != 0.0)
    {
        m_distance -= static_cast<float>(m_scrollOffset) * m_zoomSpeed * deltaTime;
        m_distance = glm::clamp(m_distance, m_minDistance, m_maxDistance);
        m_scrollOffset = 0.0; // Reset scroll
    }

    // Update camera position
    glm::vec3 direction = glm::rotate(m_orientation, glm::vec3(0.0f, 0.0f, -1.0f));
    m_position = m_target - direction * m_distance;

    // Update the up direction
    m_upDirection = glm::rotate(m_orientation, glm::vec3(0.0f, 1.0f, 0.0f));

    // Update the view matrix
    m_viewMatrix = glm::lookAt(m_position, m_target, m_upDirection);
}

void Camera::scrollCallback(double xOffset, double yOffset)
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
