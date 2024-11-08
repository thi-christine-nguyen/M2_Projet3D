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
#include <TP1/Camera/Camera_Helper.hpp>

enum class InputMode
{
	Drone,
	Fixed,
	Free, 
	Follow
};

class Camera
{

private:

	//Camera parameters 
	float		m_fovDegree{ 45.0f };
	float		m_translationSpeed;
	float 		m_rotationSpeed;
	glm::vec3	m_position{ glm::vec3(0.f, 40.f, 0.f) };
	glm::vec3	m_eulerAngle{ glm::vec3(-90.f, 0.f, 0.f) };
	glm::quat	m_rotation{};

    // Directions de la caméra
    glm::vec3 m_target;
    glm::vec3 m_rightDirection;
    glm::vec3 m_upDirection;

	//Interface option
	bool m_showImguiDemo{ false };

	// Input Mode
    std::array<const char*, 4> inputModes = { "Drone", "Fixed", "Free", "Follow"};
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

 	Camera(){
        init();
    }

	void init()
	{
		m_fovDegree = 45.0f;
		m_position = glm::vec3(0.f, 40.f, 0.f);
		m_eulerAngle = glm::vec3(glm::radians(-90.f), -glm::radians(90.f), 0.f);
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
	}

	void setCameraTarget(glm::vec3 target){
		m_target = target; 
	}

	glm::vec3 getCameraTarget(){
		return m_target; 
	}

	InputMode getInputMode() {return m_inputMode;}

	void updateInterfaceCamera(float _deltaTime)
	{
		// ImGUI window creation
		if (ImGui::BeginTabItem("Camera Settings"))
		{
			ImGui::Separator();
			ImGui::Text("Welcome to this TP about Cameras! Press escape to close the exe");
			ImGui::Text("Long live to the cameras");
			ImGui::Separator();
			ImGui::Checkbox("show Imgui Demo", &m_showImguiDemo);

			// Affichage/Modification de la position de la caméra
			ImGui::Text("Camera Position");
			ImGui::DragFloat3("##CameraPosition", glm::value_ptr(m_position));

			// Affichage/Modification du pitch et du yaw de la caméra
			ImGui::Text("Camera Orientation (Pitch, Yaw)");
			ImGui::SliderAngle("Pitch", &m_eulerAngle.x, -90.0f, 90.0f);
			ImGui::SliderAngle("Yaw", &m_eulerAngle.y, -180.0f, 180.0f);
			

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

		// Gérer le changement de mode d'input lorsque la touche "tab" est pressée
		if (glfwGetKey(_window, GLFW_KEY_C) == GLFW_PRESS) {
			m_inputMode = InputMode::Drone;
		}
		if (glfwGetKey(_window, GLFW_KEY_X) == GLFW_PRESS) {
			m_inputMode = InputMode::Follow;
		}
		if (glfwGetKey(_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			// cameraShake->startShake();
		}


		if (m_inputMode == InputMode::Drone) {  // Gestion des entrées utilisateur pour la translation de la caméra
			// Gestion des entrées utilisateur pour la translation de la caméra
			m_target = glm::vec3(0.0f, 0.0f, -1.0f);
			if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS) {
				// Avancer dans le plan horizontal de la caméra
				m_position -= glm::normalize(glm::vec3(m_target.x, 0.0f, m_target.z)) * m_translationSpeed * _deltaTime;
			}
			if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS) {
				// Reculer dans le plan horizontal de la caméra
				m_position += glm::normalize(glm::vec3(m_target.x, 0.0f, m_target.z)) * m_translationSpeed * _deltaTime;
			}
			if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS) {
				// Déplacer vers la gauche
				m_position += m_rightDirection * m_translationSpeed * _deltaTime;
			}
			if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS) {
				// Déplacer vers la droite
				m_position -= m_rightDirection * m_translationSpeed * _deltaTime;
			}
			// if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
			// 	// Déplacer vers le bas
			// 	m_position += m_target * m_translationSpeed * _deltaTime;
			// }
			// if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
			// 	// Déplacer vers le haut
			// 	m_position -= m_target * m_translationSpeed * _deltaTime;
			// }

			// Modification hauteur
			if (glfwGetKey(_window, GLFW_KEY_E) == GLFW_PRESS) {
				m_position.y -= translationSpeed;
			}
			if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
				m_position.y += translationSpeed;
			}

			// Rotation
			if (glfwGetKey(_window, GLFW_KEY_LEFT) == GLFW_PRESS) {
				m_eulerAngle.y += rotationSpeed;
				// m_eulerAngle.y = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.y), 180);
				
			}
			if (glfwGetKey(_window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
				m_eulerAngle.y -= rotationSpeed;
				// m_eulerAngle.y = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.y), 180);

			}
			if (glfwGetKey(_window, GLFW_KEY_UP) == GLFW_PRESS) {
				m_eulerAngle.x -= rotationSpeed;
				// m_eulerAngle.x = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.x), 90);
			}
			if (glfwGetKey(_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
				m_eulerAngle.x += rotationSpeed;
				// m_eulerAngle.x = Camera_Helper::clipAngle(glm::degrees(m_eulerAngle.x), 90);
			}
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

		/*if (m_inputMode == InputMode::Follow) {
			glm::vec3 cameraOffset = glm::vec3(-5.0f, 3.0f, 0.0f); // Ajustez le décalage selon vos préférences
			glm::vec3 targetPosition = m_target; // Position actuelle du joueur
			glm::vec3 cameraPosition = targetPosition + cameraOffset;
			
			// Définir la nouvelle position de la caméra
			m_position = cameraPosition;

			glm::vec3 direction = glm::normalize(targetPosition - cameraPosition);
    		m_eulerAngle.z = glm::degrees(atan2(-direction.x, -direction.z)); // Angle horizontal
    		m_eulerAngle.x = glm::degrees(asin(-direction.y)); // Angle vertical
		}*/
		if (m_inputMode == InputMode::Follow) {
			glm::vec3 cameraOffset = glm::vec3(-5.0f, 3.0f, 0.0f); // Ajustez le décalage selon vos préférences
			glm::vec3 targetPosition = m_target; // Position actuelle du joueur
			glm::vec3 cameraPosition = targetPosition + cameraOffset;
			
			if (glfwGetKey(_window, GLFW_KEY_Q) == GLFW_PRESS) {
				// Définir les paramètres de l'orbite
				float orbitRadius = 4.0f; // Rayon de l'orbite
				float orbitSpeed = 0.2f; // Vitesse de rotation en degrés par seconde

				// Calculer l'angle de rotation actuel en fonction du temps écoulé
				float currentAngle = orbitSpeed * _deltaTime;

				// Mettre à jour les angles d'Euler de la caméra pour l'orbite
				m_eulerAngle.x += currentAngle; // Rotation autour de l'axe vertical (Y)

				// Convertir les angles d'Euler en radians
				float yaw = glm::radians(m_eulerAngle.y);
				float pitch = glm::radians(m_eulerAngle.x);

				// Calculer les nouvelles coordonnées cartésiennes de la caméra
				float newX = m_target.x + orbitRadius * glm::cos(pitch);
				float newZ = m_target.z + orbitRadius * glm::sin(pitch);

				// Mettre à jour la position de la caméra
				m_position = glm::vec3(newX, m_position.y, newZ);

				// Faire en sorte que la caméra regarde toujours vers le point cible
				// Notez que cette partie reste la même que dans votre code initial
				glm::vec3 direction = glm::normalize(m_target - m_position);
				m_eulerAngle.z = glm::degrees(atan2(-direction.x, -direction.z)); // Angle horizontal
				m_eulerAngle.x = glm::degrees(asin(-direction.y)); // Angle vertical
			}


			else{
				// Calculer la nouvelle position de la caméra en utilisant l'interpolation
				glm::vec3 interpolatedCameraPosition = interpolate(m_position, targetPosition + cameraOffset, _deltaTime*4);
				// Définir la nouvelle position de la caméra
				m_position = interpolatedCameraPosition;
				glm::vec3 direction = glm::normalize(targetPosition - cameraPosition);
				float rotz = glm::degrees(atan2(-direction.x, -direction.z)); // Angle horizontal
				float rotx = glm::degrees(asin(-direction.y)); // Angle vertical
				glm::vec3 rota  = glm::vec3(rotx, m_eulerAngle.y, rotz);
				glm::vec3 interpolatedCameraRotation = interpolateRotation(m_eulerAngle, rota, _deltaTime*4);
				
				m_eulerAngle = interpolatedCameraRotation;
				m_eulerAngle.x = glm::clamp(m_eulerAngle.x, -89.0f, 89.0f);
			}

		}



		// Limiter l'angle de pitch entre -90 et 90 degrés pour éviter les retournements
		// 
		
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
		glm::vec3 resetPosition = glm::vec3(0.f, 40.f, 0.f);
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
		if(m_inputMode == InputMode::Follow){
			mat_v = glm::lookAt(m_position, m_position + m_target, m_upDirection);
			// mat_v = glm::lookAt(m_target);
		}else {
			mat_v = glm::lookAt(m_position, m_position + m_target, m_upDirection);;
		}
        
        GLuint id_v = glGetUniformLocation(programID, "view_mat");
        glUniformMatrix4fv(id_v, 1, false, &mat_v[0][0]);

        // Projection matrix : 45 Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 mat_p = glm::perspective(glm::radians(m_fovDegree), (float)4 / (float)3, 0.1f, 100.0f);
        GLuint id_p = glGetUniformLocation(programID, "project_mat");
        glUniformMatrix4fv(id_p, 1, false, &mat_p[0][0]);
    }

};

#endif
