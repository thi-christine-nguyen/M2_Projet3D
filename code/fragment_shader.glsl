#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;    
    float shininess;
}; 

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

// Uniforms
uniform Material material;
uniform Light light;
uniform vec3 viewPos;

// La texture à utiliser pour cet objet
uniform sampler2D gameObjectTexture;
uniform vec4 color;
uniform int textureID;

// Input data
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

// Output data
out vec4 fragColor;

void main(){
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // Combine results
    vec3 result = ambient + diffuse + specular;

    // Apply texture if available
    if (textureID == 0) // Pas de texture, couleur simple
        fragColor = vec4(result, 1.0) * color;
    else
        fragColor = vec4(result, 1.0) * texture(gameObjectTexture, TexCoord);
}
