#version 330 core

// Entrées depuis le Geometry Shader
in vec3 fNormal;          // Normale pour chaque face
in vec3 fWorldPosition;   // Position dans l'espace monde

// Uniformes pour l'éclairage
uniform vec3 lightPos;    // Position de la lumière dans l'espace monde
uniform vec3 viewPos;     // Position de la caméra dans l'espace monde
uniform vec3 lightColor;  // Couleur de la lumière
uniform vec3 objectColor; // Couleur de l'objet

// Entrées depuis le Geometry Shader
// flat in float isSelected;
flat in int isSelected;  // Valeur venant du Geometry Shader

// Sortie
out vec4 FragColor;

void main() {
    // Normalisation des entrées
    vec3 norm = normalize(fNormal);
    vec3 lightDir = normalize(lightPos - fWorldPosition);
    vec3 viewDir = normalize(viewPos - fWorldPosition);

    // Calcul de l'éclairage ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // Calcul de l'éclairage diffus
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Calcul de l'éclairage spéculaire
    float specularStrength = 0.5;
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Combinaison des résultats
    vec3 result = ((isSelected != 0 ? vec3(1.0, 0.0, 0.0) : ambient) + diffuse + specular) * objectColor;

    // Couleur finale
    FragColor = vec4(result, 1.0);
}
