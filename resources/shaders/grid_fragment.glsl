#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform vec3 viewPos;

void main()
{
    vec3 pos = WorldPos;
    if (abs(pos.x - round(pos.x)) < 0.03 && abs(pos.y - round(pos.y)) < 0.03 && abs(pos.z - round(pos.z)) < 0.03) {
        pos = vec3(1.0);
    }
    else {
        pos = vec3(0.3);
    }


    FragColor = vec4(pos, 1.0); // set all 4 vector values to 1.0
}