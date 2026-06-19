#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <shaders/shader.hpp>
#include <camera/camera.hpp>

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

struct Sphere
{
	glm::vec3 center{ 0.f, 0.f, 0.f };
	float radius{ 0.f };
    glm::vec3 localCenter{ 0.f, 0.f, 0.f };
    float originalRadius{ 0.f };
};

class Mesh{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        Sphere boundingSphere;
        Shader *shader;
        string vertexShaderPath;
        string fragmentShaderPath;
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Shader *shader, string vertexShaderPath, string fragmentShaderPath, Sphere boundingSphere) : vertices(vertices), indices(indices), textures(textures), shader(shader), vertexShaderPath(vertexShaderPath), fragmentShaderPath(fragmentShaderPath), boundingSphere(boundingSphere) {setupMesh();};
        void Draw(Camera &camera, glm::mat4 modelMatrix, glm::mat4 projection, glm::mat4 viewMatrix);
        void reloadShaders(string vertexShaderPath, string fragmentShaderPath);
        void reloadShaders();
        bool isOnOrForwardPlane(Plane &plane, Sphere &sphere);
        bool isOnFrustum(Frustum &camFrustum, Sphere &sphere);
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif // MESH_HPP