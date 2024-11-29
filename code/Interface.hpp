#ifndef INTERFACE_HPP__
#define INTERFACE_HPP__

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/ImGuiFileDialog.h>
#include <iostream>
#include <vector>
#include "Camera.hpp"
#include "Mesh.hpp"
#include "SceneManager.hpp"

class Interface
{
public : 

    Camera *camera; 
    Shader shader;
    SceneManager *SM;

    Interface(Shader _shader, SceneManager *_SM, Camera *_camera) :
    shader(_shader),
    SM(_SM),
    camera(_camera) {}

    void initImgui(GLFWwindow *window);
    void createFrame();
    void renderFrame();
    // Deletes all ImGUI instances
    void deleteFrame();

    void addGameObject(float _deltaTime, GLFWwindow* _window);
    void updateInterface(float _deltaTime, GLFWwindow* _window);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void update(float _deltaTime, GLFWwindow* _window);

}; 
#endif