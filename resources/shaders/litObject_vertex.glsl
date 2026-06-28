#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//for instancing we could do one of two things
//1. we could pas an array of model matrices to this shader
//2. we could pass three arrays and create the model matrix in shader to save cpu cycles

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}