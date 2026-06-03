#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 viewPos;

void main()
{
    FragColor = vec4(1.0, 0.0, 1.0, 1.0); // set all 4 vector values to 1.0
}