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

} typedef SceneTreeNode;


vector<SceneTreeNode *> hashTable[HASH_TABLE_SIZE];
SceneTreeNode *rootNode;
SceneTreeNode *sceneRootNode;
SceneTreeNode *selectedNode = nullptr;
SceneTreeNode *previousSelectedNode = nullptr;


SceneTreeNode *runtimeTextureTargetNode = nullptr;
unsigned int runtimeTestTextureID = 0;
int runtimeTestTextureUnit = 15;

void insertSceneTreeNode(SceneTreeNode *node){
    
    unsigned int slot = node->hashID % HASH_TABLE_SIZE;

    hashTable[slot].push_back(node);
}

void setTreeNode(Model* model, SceneTreeNode* node, SceneTreeNode* parent, Transform transform, string name){
    node->NodeModel = model;
    node->parentNode = parent;
    node->transform = transform;
    node->name = name;

    parent->childrenInstances.push_back(node);


    //get a hash for the node
    std::hash<std::string> str_hash;
    std::hash<float> float_hash;

    size_t h = str_hash(node->name);
    h ^= float_hash(transform.position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.position.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(hashTable[h%HASH_TABLE_SIZE].size()) + 0x9e3779b9 + (h << 6) + (h >> 2);

    node->hashID = h;
    parent->childrenHash.push_back(node->hashID);


}

void setTreeNode(Model* model, SceneTreeNode* node, Transform transform, string name, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer){
    node->NodeModel = model;

    node->transform = transform;
    node->name = name;
    node->hashID = hashID;
    node->childrenHash = childrenHash;
    node->NodeModel->removeFromDepthBuffer = removeFromDepthBuffer;
}

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, SceneTreeNode* parent) {
    Model* test = new Model(modelPath.c_str(), vertexShader.c_str(), fragmentShader.c_str(), modelName);
    SceneTreeNode *newNode = new SceneTreeNode();

    if (modelName.empty()) {
        delete test;
        delete newNode;
        return;
    }

    setTreeNode(test, newNode, parent, transform, modelName);
    insertSceneTreeNode(newNode);
}

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer) {
    Model* test = new Model(modelPath.c_str(), vertexShader.c_str(), fragmentShader.c_str(), modelName);
    SceneTreeNode *newNode = new SceneTreeNode();

    if (modelName.empty()) {
        delete test;
        delete newNode;
        return;
    }

    setTreeNode(test, newNode, transform, modelName, hashID, childrenHash, removeFromDepthBuffer);
    insertSceneTreeNode(newNode);
}

void drawSceneNode(SceneTreeNode* node, Camera &camera, glm::mat4 projection, glm::mat4 view){
    if(node == nullptr || node->NodeModel == nullptr){
        return;
    }

    // Draw the model

    node->NodeModel->transform.position = node->transform.position;
    node->NodeModel->transform.rotation = node->transform.rotation;
    node->NodeModel->transform.scale = node->transform.scale;


    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, node->transform.position);
    modelMatrix = glm::scale(modelMatrix, node->transform.scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    node->NodeModel->transform.modelMatrix = modelMatrix;

    node->NodeModel->Draw(camera, projection, view, node->NodeModel->transform.modelMatrix);
}

void removeNodeFromSceneTree(SceneTreeNode* nodeToDelete){
    if(nodeToDelete == nullptr){
        return;
    }

    // Remove the node from its parent's children

    //get the children of the node
    auto &children = nodeToDelete->childrenInstances;
    for(auto &child : children){
        child->parentNode = nodeToDelete->parentNode;
        nodeToDelete->parentNode->childrenInstances.push_back(child);
    }

    if(nodeToDelete->parentNode != nullptr){
        auto &siblings = nodeToDelete->parentNode->childrenInstances;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), nodeToDelete), siblings.end());
        nodeToDelete->parentNode->childrenHash.erase(std::remove(nodeToDelete->parentNode->childrenHash.begin(), nodeToDelete->parentNode->childrenHash.end(), nodeToDelete->hashID), nodeToDelete->parentNode->childrenHash.end());
    }

    // Remove the node from the hash table
    unsigned int slot = nodeToDelete->hashID % HASH_TABLE_SIZE;
    auto &nodes = hashTable[slot];
    nodes.erase(std::remove(nodes.begin(), nodes.end(), nodeToDelete), nodes.end());

    // Delete the node
    delete nodeToDelete;

}


// we are gonna need a getNodeByHash function
SceneTreeNode* getNodeByHash(size_t hashID) {
    unsigned int slot = hashID % HASH_TABLE_SIZE;
    auto &nodes = hashTable[slot];
    for (auto &node : nodes) {
        if (node->hashID == hashID) {
            return node;
        }
    }
    return nullptr;
}
#endif