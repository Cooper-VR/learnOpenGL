#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <shaders/shader.hpp>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 TexCoords2;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;

    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture{
    unsigned int id;
    string type;
    string path;
    string uniformName;
};



class Mesh{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        Shader *shader;
        string vertexShaderPath;
        string fragmentShaderPath;
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Shader *shader, string vertexShaderPath, string fragmentShaderPath) : vertices(vertices), indices(indices), textures(textures), shader(shader), vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath) {setupMesh();};
        void Draw(glm::mat4 modelMatrix, glm::mat4 projection, glm::mat4 viewMatrix);
        void reloadShaders(string vertexShaderPath, string fragmentShaderPath);
        void reloadShaders();
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif // MESH_HPP