#version 330 core

layout(location = 0) in vec3 inCenter;  // Le centre du voxel dans l'espace modèle
layout(location = 1) in float inHalfSize;  // La halfSize du voxel

uniform mat4 model;       // Matrice modèle (espace local -> monde)
uniform mat4 view;        // Matrice vue (monde -> caméra)
uniform mat4 projection;  // Matrice projection (caméra -> écran)

out VS_OUT {
    vec3 center;      // Centre transformé dans l'espace monde
    float halfSize;   // Taille du voxel, inchangée
} vs_out;

void main() {
    // Transformer le centre du voxel dans l'espace monde
    vec4 worldCenter = model * vec4(inCenter, 1.0);

    // Transmettre les données au geometry shader
    vs_out.center = worldCenter.xyz;  // Centre transformé
    vs_out.halfSize = inHalfSize;   // Taille, directement transmise

    // Calculer la position finale dans l'espace écran pour OpenGL
    gl_Position = projection * view * worldCenter;
}
