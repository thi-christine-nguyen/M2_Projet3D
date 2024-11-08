#version 330 core

// La texture à utiliser pour cet objet
uniform sampler2D gameObjectTexture;
uniform vec4 color;
uniform int textureID;

// Input data
in vec2 TexCoord;

// Ouput data
out vec4 fragColor;


void main(){
        // Utiliser la texture spécifiée pour cet objet en fonction des coordonnées de texture
        if (textureID == 0) // Pas de texture, couleur simple
                fragColor = color;
        else
                fragColor = texture(gameObjectTexture, TexCoord);
}
