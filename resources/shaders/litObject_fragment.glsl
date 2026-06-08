#version 330 core
out vec4 FragColor;

uniform vec3 mainColor;
uniform sampler2D testTexture;
in vec2 TexCoords;


void main()
{
    FragColor = vec4(vec3(texture(testTexture, TexCoords)), 1.0); 
}