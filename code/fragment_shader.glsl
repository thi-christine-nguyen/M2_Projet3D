#version 330 core

uniform sampler2D gameObjectTexture;
uniform vec4 color;
uniform int textureID;

in vec2 TexCoord;
in vec3 FragPos;   // Position du fragment depuis le vertex shader
in vec3 Normal;    // Normale du fragment depuis le vertex shader

// Uniformes pour la lumière et le matériau
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

out vec4 fragColor;

void main() {
    // Propriétés de la lumière
    vec3 ambient = lightColor * material.ambient;

    // Calcul de la lumière diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * material.diffuse;

    // Calcul de la lumière spéculaire
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * lightColor * material.specular;

    // Combinaison des composants
    vec3 phong = ambient + diffuse + specular;

    // Application de la texture ou de la couleur
    if (textureID == 0)
        fragColor = vec4(phong * color.rgb, 1.0);
    else
        fragColor = vec4(phong, 1.0) * texture(gameObjectTexture, TexCoord);
}