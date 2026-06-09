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
};

class Model{
    public:
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, bool gammaCorrection = false);
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool gammaCorrection = false);
        void Draw(glm::mat4 projection, glm::mat4 viewMatrix, glm::mat4 modelMatrix);
        void reloadShader();
        unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);

        Shader *shader;
        vector<string> names;
        string directory;
        string vertexShaderPath;
        string fragmentShaderPath;
        vector<Texture> textures_loaded;
        int numberOfVertices = 0;
        int numberOfBatches = 0;
        vector<Mesh> meshes;
    private:
        bool gammaCorrection;
        
        void loadModel(string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        
};

#endif
