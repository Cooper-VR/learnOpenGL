#ifndef MODEL_H
#define MODEL_H

#include <model/mesh/mesh.hpp>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags

class Model{
    public:
    Model (char* path) {loadModel(path);}
    void Draw(Shader &shader) {for(int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);}
    private:
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char *path, const string &directory);
};

#endif