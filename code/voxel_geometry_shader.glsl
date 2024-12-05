#version 330 core

layout(points) in;                              // Chaque primitive est un point
layout(triangle_strip, max_vertices = 36) out; // Chaque voxel produit jusqu'à 36 sommets (6 faces * 2 triangles/face)

in VS_OUT {
    vec4 center;      // Centre en espace local
    float halfSize;   // Taille (demi-côté du voxel)
} gs_in[];

// Sorties pour le fragment shader
out vec3 fNormal;  // Normale pour chaque face
out vec3 fWorldPosition; // Position dans l'espace monde (pour debug ou shading)

// Uniformes pour les matrices
uniform mat4 model;         // Matrice modèle
uniform mat4 view;          // Matrice vue
uniform mat4 projection;    // Matrice projection

// Offsets relatifs pour construire les sommets d'un cube
const vec3 offsets[8] = vec3[](
    vec3(-1, -1, -1),  // Bas arrière gauche
    vec3( 1, -1, -1),  // Bas arrière droite
    vec3( 1,  1, -1),  // Haut arrière droite
    vec3(-1,  1, -1),  // Haut arrière gauche
    vec3(-1, -1,  1),  // Bas avant gauche
    vec3( 1, -1,  1),  // Bas avant droite
    vec3( 1,  1,  1),  // Haut avant droite
    vec3(-1,  1,  1)   // Haut avant gauche
);

// Indices des sommets pour former les 6 faces (quads)
const int faces[24] = int[](
    // Arrière
    0, 1, 2, 3,
    // Avant
    4, 5, 6, 7,
    // Gauche
    0, 4, 7, 3,
    // Droite
    1, 5, 6, 2,
    // Haut
    3, 2, 6, 7,
    // Bas
    0, 1, 5, 4
);

// Normales des 6 faces
const vec3 normals[6] = vec3[](
    vec3( 0,  0, -1),  // Arrière
    vec3( 0,  0,  1),  // Avant
    vec3(-1,  0,  0),  // Gauche
    vec3( 1,  0,  0),  // Droite
    vec3( 0,  1,  0),  // Haut
    vec3( 0, -1,  0)   // Bas
);

void main() {
    // Récupérer les données d'entrée
    float halfSize = gs_in[0].halfSize; // Taille (demi-côté)
    vec3 worldCenter = (model * gs_in[0].center).xyz; // Centre en espace monde

    // Parcourir les 6 faces
    for (int i = 0; i < 6; ++i) {
        fNormal = normals[i]; // Normale de la face
        fWorldPosition = worldCenter; // Centre dans l'espace monde (pour debug)

        // Émettre les 4 sommets pour chaque face
        for (int j = 0; j < 4; ++j) {
            int idx = faces[i * 4 + j];             // Récupérer l'indice
            vec3 localOffset = halfSize * offsets[idx]; // Offset dans l'espace local
            vec3 worldPosition = worldCenter + localOffset; // Position finale dans l'espace monde
            gl_Position = projection * view * vec4(worldPosition, 1.0); // Transformer en clip space
            EmitVertex(); // Émettre un sommet
        }

        EndPrimitive(); // Terminer la face
    }
}
