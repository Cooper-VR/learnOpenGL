#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: %s\n", path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer (+1 for null terminator)
    char* buffer = (char*)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "ERROR::SHADER::MEMORY_ALLOCATION_FAILED\n");
        fclose(file);
        return NULL;
    }

    // Read file contents
    size_t readLen = fread(buffer, 1, length, file);
    buffer[readLen] = '\0'; // null-terminate

    fclose(file);
    return buffer;
}

static unsigned int compile_shader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader compilation error: %s\n", infoLog);
    }
    return shader;
}

Shader shader_create(const char* vertexSrc, const char* fragmentSrc) {
    Shader shader;

    char* vertexSource = read_file(vertexSrc);
    char* fragmentSource = read_file(fragmentSrc);

    unsigned int vertex = compile_shader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragment = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);

    shader.ID = glCreateProgram();
    glAttachShader(shader.ID, vertex);
    glAttachShader(shader.ID, fragment);
    glLinkProgram(shader.ID);

    int success;
    glGetProgramiv(shader.ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shader.ID, 512, NULL, infoLog);
        fprintf(stderr, "Shader link error: %s\n", infoLog);
    }

    free(vertexSource);
    free(fragmentSource);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return shader;
}

void shader_use(Shader* shader) {
    glUseProgram(shader->ID);
}

void shader_delete(Shader* shader) {
    glDeleteProgram(shader->ID);
}

void shader_setBool(Shader* shader, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), (int)value);
}

void shader_setInt(Shader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void shader_setFloat(Shader* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void shader_setVec3(Shader* shader, const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader->ID, name), x, y, z);
}

void shader_setMat4(Shader* shader, const char* name, const float* mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, mat);
}
