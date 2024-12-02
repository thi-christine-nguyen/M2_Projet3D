#ifndef CAMERA_HPP__
#define CAMERA_HPP__

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
#include "Camera_Helper.hpp"

enum class InputMode
{
	Fixed,
	Free
};

class Camera
{

private:

	//Camera parameters 
	float		m_fovDegree{ 45.0f };
	float		m_translationSpeed;
	float 		m_rotationSpeed;
	glm::vec3	m_position{ glm::vec3(0.f, 0.f, 5.f) };
	glm::vec3	m_eulerAngle = glm::vec3(0.f, 0.f, 0.f);
	glm::quat	m_rotation{};

    // Directions de la caméra
    glm::vec3 m_target;
    glm::vec3 m_rightDirection;
    glm::vec3 m_upDirection;

	//Interface option
	bool m_showImguiDemo{ false };

	// Input Mode
    std::array<const char*, 2> inputModes = { "Fixed", "Free"};
	InputMode m_inputMode;

	// mouseInputs
    double m_lastMouseX;
    double m_lastMouseY;

	//View
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;

	// smoothTransitions
    bool m_transitioning; // Indique si une transition est en cours
    double m_transitionElapsedTime; // Temps au début de la transition
    float m_transitionDuration; // Durée de la transition

public: 
	// Attributs sauvegardés
	glm::vec3 m_savedPosition;
	glm::vec3 m_savedEulerAngle;
	glm::quat m_savedRotation;
	InputMode m_savedInputMode;
	bool m_stateSaved{ false };

 	Camera(){
        init();
    }

	void init();

    void saveState(bool saveInputMode = true);

    // Fonction pour restaurer l'état sauvegardé
    void restoreState();

	void setCameraTarget(glm::vec3 target){
		m_target = target; 
	}

	glm::vec3 getCameraTarget(){
		return m_target; 
	}

	InputMode getInputMode() {return m_inputMode;}
	void setInputMode(InputMode _inputMode) {
		m_inputMode = _inputMode;
	}

	glm::vec3 getPosition(){
		return m_position; 
	}

	void updateInterfaceCamera(float _deltaTime);
	
	void updateFreeInput(float _deltaTime, GLFWwindow* _window);
	void updateCameraRotation();
	glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float ratio);

	// Fonction pour effectuer une interpolation de rotation en utilisant les angles d'Euler
	glm::vec3 interpolateRotation(const glm::vec3& startAngles, const glm::vec3& endAngles, float ratio);

	void resetWithTransition(float _deltaTime);
	void update(float _deltaTime, GLFWwindow* _window);
	glm::quat getRotation() const {return m_rotation;}
	glm::mat4 getViewMatrix() const {return m_viewMatrix;}
	glm::mat4 getProjectionMatrix() const {return m_projectionMatrix;}
	void sendToShader(GLuint programID);
	void scrollCallback(double xOffset, double yOffset);

};

#endif
