#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertices_position_modelspace;
layout (location = 1) in vec2 textureCoordinates;

uniform mat4 view_mat;
uniform mat4 project_mat;

uniform int type;
uniform mat4 transform;

out vec2 TexCoord; // UV

void main() {
	vec3 pos = vertices_position_modelspace;
	TexCoord = textureCoordinates; // Récupération des UVs pour l'envoyer au fragment shader

        gl_Position = project_mat * view_mat * transform * vec4(pos, 1);
}

