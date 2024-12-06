#version 330 core

layout(location = 0) in vec3 inCenter;  // Le centre du voxel dans l'espace modèle
layout(location = 1) in float inHalfSize;  // La halfSize du voxel
layout(location = 2) in int inIsEmpty;  // Est-ce que le voxel est plein ou pas

out VS_OUT {
    vec4 center;      // Centre transformé dans l'espace monde
    float halfSize;   // Taille du voxel, inchangée
    int isEmpty;
} vs_out;

void main() {
    // Transmettre les données au geometry shader
    vs_out.center = vec4(inCenter, 1.0);  // Centre transformé
    vs_out.halfSize = inHalfSize;   // Taille, directement transmise
    vs_out.isEmpty = inIsEmpty;

    // Calculer la position finale dans l'espace écran pour OpenGL
    // gl_Position = projection * view * worldCenter;
}
