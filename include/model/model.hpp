#ifndef MODEL_H
#define MODEL_H
#include <glm/gtc/matrix_transform.hpp>
#include <model/mesh/mesh.hpp>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags
#include <loaders/stb_image.h>
#include <filesystem>
#include <functional>
#include <string>
#include <iostream>

#define HASH_TABLE_SIZE 1024

struct Transform{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 modelMatrix;
};


class Model{
    public:
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, bool gammaCorrection = false);
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool gammaCorrection = false);
        void Draw(Camera &camera, glm::mat4 projection, glm::mat4 viewMatrix, glm::mat4 modelMatrix);
        void reloadShader(string vertexShaderPath, string fragmentShaderPath);
        unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);

        //Shader *shader;
        vector<string> names;
        Transform transform;
        string directory;
        int numberOfVertices = 0;
        int numberOfBatches = 0;
        vector<Mesh> meshes;
        bool removeFromDepthBuffer = false;
    private:
        bool gammaCorrection;

        void loadModel(string const &path, string vertexShaderPath, string fragmentShaderPath);
        void processNode(aiNode *node, const aiScene *scene, string vertexShaderPath, string fragmentShaderPath);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene, string vertexShaderPath, string fragmentShaderPath);

        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        
};

#endif
