#ifndef MODEL_H
#define MODEL_H

#include <model/mesh/mesh.hpp>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags
#include <loaders/stb_image.h>
#include <iostream>

class Model{
    public:
    Model (const char* path, bool gammaCorrection = false) : gammaCorrection(gammaCorrection) {loadModel(path);}
    void Draw(Shader &shader) {for(unsigned int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);}
    private:
    bool gammaCorrection;
    vector<Texture> textures_loaded;
    vector<Mesh> meshes;
    string directory;

    void loadModel(string const &path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
    unsigned int TextureFromFile(const char *path, const string &directory, bool gamma);
};

#endif