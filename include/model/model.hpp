#include <model/mesh/mesh.hpp>

class Model{
    public:
    Model (char* path) {loadModel(path);}
    void Draw(Shader &shader) {for(int i = 0; i < meshes.size(); i++) meshes[i].Draw(shader);}
    private:
    vector<Mesh> meshes;
    string directory;

    void loadModel(std::string path);
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);
};