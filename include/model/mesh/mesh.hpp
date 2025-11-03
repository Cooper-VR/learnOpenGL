#ifndef MESH_HPP
#define MESH_HPP

#include <glm/glm.hpp>
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
};

class Mesh{
    public:
        vector<Vertex> vertices;
        vector<unsigned int> indices;
        vector<Texture> textures;
        Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) : vertices(vertices), indices(indices), textures(textures) {setupMesh();};
        void Draw(Shader &shader);
    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

#endif // MESH_HPP