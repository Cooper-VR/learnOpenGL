#ifndef MODEL_H
#define MODEL_H
#include <glm/gtc/matrix_transform.hpp>
#include <model/mesh/mesh.hpp>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags
#include <loaders/stb_image.h>
#include <iostream>
#include <filesystem>

struct Transform{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

class Model{
    public:
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, bool gammaCorrection = false);
        Model (const char* path, const char* vertexShader, const char* fragShader, string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool gammaCorrection = false);
        int addInstance(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string name = "empty");
        void Draw(glm::mat4 projection, glm::mat4 viewMatrix);
        void reloadShader();

        Shader *shader;
        vector<size_t> Hash_ID;
        unsigned int instanceCount = 0;
        vector<string> names;
        vector<Transform> transforms;
        vector<glm::mat4> modelMatrix;
        string directory;
    private:
        bool gammaCorrection;
        vector<Texture> textures_loaded;
        vector<Mesh> meshes;

        void loadModel(string const &path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);

        vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
        unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
};

#endif
