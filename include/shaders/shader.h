#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct{
    unsigned int ID; //the program ID
} Shader;

Shader shader_create(const char* vertexSrc, const char* fragmentSrc);
void shader_use(Shader* shader);
void shader_delete(Shader* shader);

// uniform helpers
void shader_setBool(Shader* shader, const char* name, bool value);
void shader_setInt(Shader* shader, const char* name, int value);
void shader_setFloat(Shader* shader, const char* name, float value);
void shader_setVec3(Shader* shader, const char* name, float x, float y, float z);
void shader_setMat4(Shader* shader, const char* name, const float* mat);
#endif