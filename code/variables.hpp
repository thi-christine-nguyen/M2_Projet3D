#ifndef VARIABLES_H_
#define VARIABLES_H_


#include "Camera/Camera.hpp"
#include "Interface.hpp"

// Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// General
GLFWwindow *window;
GLuint programID;

// Timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// Camera


// For speed computation
double lastTime = glfwGetTime();
double totalDeltaTime = 0.;
double physicsClock = 0.;
int nbFrames = 0;

// Get a handle for our "Model View Projection" matrices uniforms
glm::mat4 mat_m = glm::mat4(1); // Model

#endif