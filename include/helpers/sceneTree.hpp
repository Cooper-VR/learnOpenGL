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
    int hashID;

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
    int slot = node->hashID;

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

    h %= HASH_TABLE_SIZE;

    node->hashID = h;


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

void drawSceneNode(SceneTreeNode* node, glm::mat4 projection, glm::mat4 view){
    if(node == nullptr || node->NodeModel == nullptr){
        return;
    }

    // Draw the model

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, node->transform.position);
    modelMatrix = glm::scale(modelMatrix, node->transform.scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    node->NodeModel->Draw(projection, view, modelMatrix);
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
    }

    // Remove the node from the hash table
    int slot = nodeToDelete->hashID;
    auto &nodes = hashTable[slot];
    nodes.erase(std::remove(nodes.begin(), nodes.end(), nodeToDelete), nodes.end());

    // Delete the node
    delete nodeToDelete;

}

#endif