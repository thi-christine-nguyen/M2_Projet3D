#version 330 core

layout(location = 0) in vec3 vertices_position_modelspace;
layout(location = 1) in vec2 textureCoordinates;
layout(location = 2) in vec3 normal_modelspace; // Nouvelle entrée pour la normale

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;

out vec2 TexCoord; // UV
out vec3 FragPos;  // Position du fragment
out vec3 Normal;   // Normale du fragment

void main() {
    FragPos = vec3(model * vec4(vertices_position_modelspace, 1.0));
    Normal = mat3(transpose(inverse(model))) * normal_modelspace;
    TexCoord = textureCoordinates;
    
    gl_Position = projection * view * model * vec4(vertices_position_modelspace, 1.0);
}
