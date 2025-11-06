#ifndef MODEL_H
#define MODEL_H
#include <glm/gtc/matrix_transform.hpp>
#include <model/mesh/mesh.hpp>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags
#include <loaders/stb_image.h>
#include <iostream>

class Model{
    public:
    Model (const char* path, const char* vertexShader, const char* fragShader, string name, bool gammaCorrection = false) : gammaCorrection(gammaCorrection) 
    {
        shader = new Shader(vertexShader, fragShader);
        loadModel(path);

        directory = path;
        if (instanceCount == 0) {
            addInstance(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), name);
        }
    }

    Model (const char* path, const char* vertexShader, const char* fragShader, string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool gammaCorrection = false) : gammaCorrection(gammaCorrection) 
    {
        shader = new Shader(vertexShader, fragShader);
        loadModel(path);
        directory = path;

        this->position.push_back(position);
        this->rotation.push_back(rotation);
        this->scale.push_back(scale);

        if (instanceCount == 0) {
            addInstance(position, rotation, scale, name);
        }
    }

    void addInstance(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, string name = "empty") {
        this->position.push_back(position);
        this->rotation.push_back(rotation);
        this->scale.push_back(scale);
        names.push_back(name);

        glm::mat4 modelMat(1.0f);
        modelMatrix.push_back(modelMat);

        std::hash<std::string> str_hash;
        std::hash<float> float_hash;

        size_t h = str_hash(name);
        h ^= float_hash(position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= float_hash(position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= float_hash(position.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= float_hash(instanceCount) + 0x9e3779b9 + (h << 6) + (h >> 2);

        Hash_ID.push_back(h);
        instanceCount++;
    }

    void Draw(glm::mat4 projection, glm::mat4 viewMatrix) {
        for(unsigned int i = 0; i < meshes.size(); i++){
            for (unsigned int j = 0; j < modelMatrix.size(); j++){
                shader->use();
                modelMatrix[j] = glm::mat4(1.0f);
                modelMatrix[j] = glm::translate(modelMatrix[j], position[j]);
                modelMatrix[j] = glm::scale(modelMatrix[j], scale[j]);
                modelMatrix[j] = glm::rotate(modelMatrix[j], glm::radians(rotation[j].x), glm::vec3(1.0f, 0.0f, 0.0f));
                modelMatrix[j] = glm::rotate(modelMatrix[j], glm::radians(rotation[j].y), glm::vec3(0.0f, 1.0f, 0.0f));
                modelMatrix[j] = glm::rotate(modelMatrix[j], glm::radians(rotation[j].z), glm::vec3(0.0f, 0.0f, 1.0f));
                shader->setMat4("projection", projection);
                shader->setMat4("view", viewMatrix);
                shader->setMat4("model", modelMatrix[j]);
                meshes[i].Draw(*shader, modelMatrix[j], projection, viewMatrix);
            }
        }
    }

    Shader *shader;
    vector<size_t> Hash_ID;
    unsigned int instanceCount = 0;
    vector<string> names;
    vector<glm::mat4> modelMatrix;
    vector<glm::vec3> position;
    vector<glm::vec3> rotation;
    vector<glm::vec3> scale;
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
