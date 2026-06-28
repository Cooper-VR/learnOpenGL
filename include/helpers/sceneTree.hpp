#ifndef SCENE_TREE_HPP
#define SCENE_TREE_HPP

#include <vector>
#include <utility>
#include <model/model.hpp>

struct SceneTreeNode{
    Model* NodeModel;
    vector<SceneTreeNode*> childrenInstances;
    SceneTreeNode* parentNode;
    string name;
    Transform transform;
    unsigned int hashID;
    vector<unsigned int> childrenHash;
    unsigned int frameID = 0;
    vector<SceneTreeNode*> instances;
    vector<SceneTreeNode> instancesOfThisNode;
    bool isAnInstance = false;

    ~SceneTreeNode() {
        if (NodeModel != nullptr) {
            delete NodeModel;
            NodeModel = nullptr;
        }
        parentNode = nullptr;
        hashID = 0;
        frameID = 0;
    }

} typedef SceneTreeNode;


extern vector<SceneTreeNode *> hashTable[HASH_TABLE_SIZE];
extern SceneTreeNode *rootNode;
extern SceneTreeNode *sceneRootNode;
extern SceneTreeNode *selectedNode;
extern SceneTreeNode *previousSelectedNode;
extern vector<SceneTreeNode*> nodesToDrawNormal;
void insertSceneTreeNode(SceneTreeNode *node);

void setTreeNode(Model* model, SceneTreeNode* node, SceneTreeNode* parent, Transform transform, string name);

void setTreeNode(Model* model, SceneTreeNode* node, Transform transform, string name, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer, bool renderNormal);

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, SceneTreeNode* parent);

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer, bool renderNormal);

bool drawSceneNode(SceneTreeNode* node, Camera &camera, glm::mat4 projection, glm::mat4 view);

void removeNodeFromSceneTree(SceneTreeNode* nodeToDelete);

SceneTreeNode* getNodeByHash(size_t hashID);

#endif