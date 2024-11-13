#ifndef CAMERA_HPP__
#define CAMERA_HPP__
#pragma once

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
#include <code/Camera/Camera_Helper.hpp>

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
	glm::vec3	m_position{ glm::vec3(8.f, 40.f, 0.f) };
	glm::vec3	m_eulerAngle{ glm::vec3(-90.f, 0.f, 0.f) };
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

	void init()
	{
		m_fovDegree = 45.0f;
		m_position = glm::vec3(8.f, 40.f, 0.f);
		m_eulerAngle = glm::vec3(glm::radians(-90.f), glm::radians(0.f), 0.f);
		m_rotation = glm::quat{};
		m_translationSpeed = 15.0f;
		m_rotationSpeed = 1.0f;
		m_target = glm::vec3(0.0f, 0.0f, -1.0f); // La caméra commence à regarder vers l'avant
		m_rightDirection = glm::vec3(1.0f, 0.0f, 0.0f); // La direction droite initiale
		m_upDirection = glm::vec3(0.0f, 1.0f, 0.0f); // La direction vers le haut initiale
		m_inputMode = InputMode::Free;
		m_lastMouseX = 0.0;
		m_lastMouseY = 0.0;

		// Transition
		m_transitionDuration = 1.0f;
		m_transitioning = false;
		m_transitionElapsedTime = 0.0f;

		saveState();
	}

    void saveState(bool saveInputMode = true) {
        m_savedPosition = m_position;
        // m_savedEulerAngle = m_eulerAngle;
		m_savedRotation = m_rotation;
		if (saveInputMode) {
			// std::cout << saveInputMode << std::endl;
        	m_savedInputMode = m_inputMode;
		}
        m_stateSaved = true;
    }

    // Fonction pour restaurer l'état sauvegardé
    void restoreState() {
        if (m_stateSaved) {
            m_position = m_savedPosition;
            // m_eulerAngle = m_savedEulerAngle;
			m_rotation = m_savedRotation;
            m_inputMode = m_savedInputMode;
            m_stateSaved = false; // Optionnel, on peut garder l'état pour une restauration future
			// updateCameraRotation();
			Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, m_fovDegree);
        }
    }

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

	void updateInterfaceCamera(float _deltaTime)
	{
		// ImGUI window creation
		if (ImGui::BeginTabItem("Camera Settings"))
		{
			ImGui::Separator();
			ImGui::Text("Press escape to close the exe");
			ImGui::Separator();

			// Affichage/Modification de la position de la caméra
			ImGui::Text("Camera Position");
			ImGui::DragFloat3("##CameraPosition", glm::value_ptr(m_position));
			if (ImGui::IsItemDeactivatedAfterEdit()) saveState(false);

			// Affichage/Modification du pitch et du yaw de la caméra
			ImGui::Text("Camera Orientation (Pitch, Yaw)");
			ImGui::SliderAngle("Pitch", &m_eulerAngle.x, -180.0f, 180.0f);
			if (ImGui::IsItemDeactivatedAfterEdit()) saveState(false);
			ImGui::SliderAngle("Yaw", &m_eulerAngle.y, -180.0f, 180.0f);
			if (ImGui::IsItemDeactivatedAfterEdit()) saveState(false);

			// Réglage de la focale (FoV) de la caméra
			ImGui::Text("Field Of View");
			ImGui::SliderFloat("Field of View", &m_fovDegree, 1.0f, 120.0f);

			// Réglage de la vitesse de translation et de rotation
			ImGui::Text("Translation and Rotation speed");
			ImGui::SliderFloat("Translation Speed", &m_translationSpeed, 0.1f, 100.0f);
			ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.1f, 10.0f);

			// Réglage de la durée de la transition
			ImGui::Text("Transition Duration");
			ImGui::SliderFloat("Transition Duration", &m_transitionDuration, 0.1f, 10.0f);

			// Afficher l'information du mode courant
			ImGui::Text("Input Mode: %s", inputModes[(int)m_inputMode]);

			// Liste déroulante pour les modes d'inputs
			int currentMode = static_cast<int>(m_inputMode);
			if (ImGui::Combo("Input Mode", &currentMode, inputModes.data(), inputModes.size())) {
				// Mettre à jour le mode d'input lorsque l'utilisateur sélectionne une option
				m_inputMode = static_cast<InputMode>(currentMode);
			}

			// Bouton de réinitialisation
			if (ImGui::Button("Reset Camera Settings")) {
				resetWithTransition(_deltaTime);
			}
			ImGui::EndTabItem();
			
		}
		

	}
	
	void updateFreeInput(float _deltaTime, GLFWwindow* _window)
	{
		// Vitesse de translation
		float translationSpeed = m_translationSpeed * _deltaTime;
		// Vitesse de rotation
		float rotationSpeed = m_rotationSpeed * _deltaTime;

		// Gérer le changement de mode d'input lorsque qu'une touche est pressée
		if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS) {
			m_inputMode = InputMode::Fixed;
		}
		if (glfwGetKey(_window, GLFW_KEY_V) == GLFW_PRESS) {
			m_inputMode = InputMode::Free;
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			// cameraShake->startShake();
		}

		if (m_inputMode == InputMode::Free) {
			// m_target = glm::vec3(0.0f, 0.0f, -1.0f);
			// Rotation de la caméra avec la souris
			double mouseX, mouseY;
			glfwGetCursorPos(_window, &mouseX, &mouseY);

			if (glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
				double deltaX = mouseX - m_lastMouseX;
				double deltaY = mouseY - m_lastMouseY;

				m_eulerAngle.x += static_cast<float>(deltaY) * m_rotationSpeed * 0.0075;
				// m_eulerAngle.x = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.x), 90);
		
		
				m_eulerAngle.y -= static_cast<float>(deltaX) * m_rotationSpeed * 0.0075;
				// m_eulerAngle.y = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.y), 180);

				
			}

			m_lastMouseX = mouseX;
			m_lastMouseY = mouseY;

			// Translation de la caméra avec les touches ZQSD
			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
				m_position -= m_target * translationSpeed;
			}
			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
				m_position += m_target * translationSpeed;
			}
			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
				m_position += m_rightDirection * translationSpeed;
			}
			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
				m_position -= m_rightDirection * translationSpeed;
			}
		}
	}
	void updateCameraRotation()
	{
		// Convertir les angles d'Euler en quaternion pour la rotation
		glm::quat pitchQuat = glm::angleAxis(m_eulerAngle.x, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat yawQuat = glm::angleAxis(m_eulerAngle.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m_rotation = yawQuat * pitchQuat;

		// Recalcul des vecteurs de directions
		m_target = glm::normalize(glm::rotate(m_rotation, glm::vec3(0.0f, 0.0f, -1.0f)));
		m_rightDirection = glm::normalize(glm::rotate(m_rotation, glm::vec3(1.0f, 0.0f, 0.0f)));
		m_upDirection = glm::normalize(glm::cross(m_rightDirection, m_target));
	}

	glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float ratio)
	{
		// Utilisation d'une interpolation cosinus pour des transitions plus douces
		ratio = (1.0f - cos(ratio * glm::pi<float>())) * 0.5f;

		// Interpolation linéaire entre les valeurs de départ et d'arrivée
		return start * (1.0f - ratio) + end * ratio;
	}

	// Fonction pour effectuer une interpolation de rotation en utilisant les angles d'Euler
	glm::vec3 interpolateRotation(const glm::vec3& startAngles, const glm::vec3& endAngles, float ratio)
	{
		// Assurez-vous que les angles sont dans la plage correcte (-180 à 180 degrés)
		glm::vec3 startNormalized = glm::degrees(glm::eulerAngles(glm::quat(glm::radians(startAngles))));
		glm::vec3 endNormalized = glm::degrees(glm::eulerAngles(glm::quat(glm::radians(endAngles))));

		// Utilisez une interpolation linéaire entre les angles de départ et d'arrivée
		glm::vec3 interpolatedAngles = startNormalized * (1.0f - ratio) + endNormalized * ratio;

		return interpolatedAngles;
	}

	void resetWithTransition(float _deltaTime) {
		// Sauvegarder l'état actuel de la caméra
		glm::vec3 currentPosition = m_position;
		glm::vec3 currentEulerAngles = m_eulerAngle;

		// Définir les nouvelles valeurs pour la réinitialisation
		glm::vec3 resetPosition = glm::vec3(8.f, 40.f, 0.f);
		glm::vec3 resetEulerAngles = glm::vec3(glm::radians(-90.f), 0.f, 0.f);

		// Définir la durée de la transition
		float transitionTime = m_transitionDuration; // Utilisez la valeur définie dans ImGui

		// Effectuer la transition en interpolant progressivement entre les états actuel et réinitialisé
		if (m_transitionElapsedTime < transitionTime) {
			m_transitioning = true;
			float currentFrame = glfwGetTime();
			float ratio = m_transitionElapsedTime / transitionTime;

			// Interpoler les positions et les angles
			glm::vec3 interpolatedPosition = interpolate(currentPosition, resetPosition, ratio);
			glm::vec3 interpolatedEulerAngles = interpolate(currentEulerAngles, resetEulerAngles, ratio);

			// std::cout << m_transitionElapsedTime << "/" << transitionTime << std::endl;

			// Mettre à jour la position et les angles de la caméra
			m_position = interpolatedPosition;
			m_eulerAngle = interpolatedEulerAngles;


			// Mettre à jour le temps écoulé
			m_transitionElapsedTime += _deltaTime;
		} else {
			m_transitioning = false;
			m_transitionElapsedTime = 0.0f;
		}

		// m_position = resetPosition;
		// m_eulerAngle = resetEulerAngles;
		// update(_deltaTime, _window);
	}

	void update(float _deltaTime, GLFWwindow* _window)
	{
		// updateInterface(_deltaTime);
		updateFreeInput(_deltaTime, _window);
		
		updateCameraRotation();
		if (m_transitioning)
			resetWithTransition(_deltaTime);

		Camera_Helper::computeFinalView(m_projectionMatrix, m_viewMatrix, m_position, m_rotation, m_fovDegree);
	}

	glm::quat getRotation() const {return m_rotation;}
	glm::mat4 getViewMatrix() const {return m_viewMatrix;}
	glm::mat4 getProjectionMatrix() const {return m_projectionMatrix;}

	void sendToShader(GLuint programID) {
        // View matrix : camera/view transformation lookat() utiliser camera_position camera_target camera_up
		glm::mat4 mat_v;
		mat_v = glm::lookAt(m_position, m_position + m_target, m_upDirection);
        
        GLuint id_v = glGetUniformLocation(programID, "view_mat");
        glUniformMatrix4fv(id_v, 1, false, &mat_v[0][0]);

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 mat_p = glm::perspective(glm::radians(m_fovDegree), (float)4 / (float)3, 0.1f, 100.0f);
        GLuint id_p = glGetUniformLocation(programID, "project_mat");
        glUniformMatrix4fv(id_p, 1, false, &mat_p[0][0]);
    }

	void scrollCallback(double xOffset, double yOffset) {
		m_translationSpeed += static_cast<float>(yOffset);
		m_translationSpeed = glm::clamp(m_translationSpeed, 0.1f, 100.0f); // Limiter la vitesse
	}

};

#endif
