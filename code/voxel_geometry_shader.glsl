#version 330 core

layout(points) in;                              // Chaque primitive est un point
layout(triangle_strip, max_vertices = 24) out; // Chaque voxel produit jusqu'à 36 sommets (6 faces * 2 triangles/face)

// Entrées depuis le Vertex Shader
in VS_OUT {
    vec4 center;      // Centre en espace local
    float halfSize;   // Taille (demi-côté du voxel)
} gs_in[];

// Sorties pour le Fragment Shader
out vec3 fNormal;          // Normale pour chaque face
out vec3 fWorldPosition;   // Position dans l'espace monde (pour debug ou shading)

// Uniformes pour les matrices
uniform mat4 model;         // Matrice modèle
uniform mat4 view;          // Matrice vue
uniform mat4 projection;    // Matrice projection

// Fonction pour émettre un sommet avec une normale
void EmitVertexWithNormal(vec3 position, vec3 normal) {
    fNormal = normalize(normal);                    // Envoie la normale au fragment shader
    fWorldPosition = position;                      // Position en espace monde
    gl_Position = projection * view * vec4(position, 1.0); // Transforme en espace clip
    EmitVertex();
}

// Fonction pour générer un cube centré sur `center` avec un rayon `halfSize`
void generateCube(vec3 center, float halfSize) {
    // Les 8 sommets du cube, définis par leur offset relatif
    vec3 offsets[8] = vec3[](
        vec3(-1, -1, -1),  // 0 : arrière-bas-gauche
        vec3( 1, -1, -1),  // 1 : arrière-bas-droit
        vec3( 1,  1, -1),  // 2 : arrière-haut-droit
        vec3(-1,  1, -1),  // 3 : arrière-haut-gauche
        vec3(-1, -1,  1),  // 4 : avant-bas-gauche
        vec3( 1, -1,  1),  // 5 : avant-bas-droit
        vec3( 1,  1,  1),  // 6 : avant-haut-droit
        vec3(-1,  1,  1)   // 7 : avant-haut-gauche
    );

    // Normales pour chaque face
    vec3 normals[6] = vec3[](
        vec3( 0,  0, -1), // Face arrière
        vec3( 0,  0,  1), // Face avant
        vec3(-1,  0,  0), // Face gauche
        vec3( 1,  0,  0), // Face droite
        vec3( 0, -1,  0), // Face dessous
        vec3( 0,  1,  0)  // Face dessus
    );

    // Indices des sommets pour toutes les faces dans un tableau 1D
    int indices[24] = int[](
        0, 1, 3, 2,  // Face arrière
        4, 5, 7, 6,  // Face avant
        0, 3, 4, 7,  // Face gauche
        1, 2, 5, 6,  // Face droite
        0, 1, 4, 5,  // Face dessous
        3, 2, 7, 6   // Face dessus
    );

    // Générer chaque face
    for (int face = 0; face < 6; ++face) {
        vec3 normal = normals[face];

        // Émettre les 4 sommets pour la face
        for (int i = 0; i < 4; ++i) {
            int vertexIndex = indices[face * 4 + i];
            vec3 vertex = center + offsets[vertexIndex] * halfSize;
            EmitVertexWithNormal(vertex, normal);
        }

        EndPrimitive(); // Terminer le triangle strip pour cette face
    }
}



void main() {
    // Calculer la position dans l'espace monde pour le centre
    vec3 worldCenter = (model * gs_in[0].center).xyz;

    // Générer un cube à partir du centre et de la demi-taille
    generateCube(worldCenter, gs_in[0].halfSize);
}
