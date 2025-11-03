#ifndef SCENE_TREE_HPP
#define SCENE_TREE_HPP

#include <vector>
#include <model/model.hpp>

struct SceneTreeNode{
    Model* NodeModel;
    unsigned int instanceCount;
    SceneTreeNode* leftChildInstance;
    SceneTreeNode* rightChildInstance;

    vector<SceneTreeNode*> childrenInstances;
    SceneTreeNode* parentNode;

} typedef SceneTreeNode;

SceneTreeNode* insertInstanceToSceneTree(SceneTreeNode* root, Model* model, unsigned int instanceIndex){
    if(root->NodeModel == nullptr){
        root->instanceCount = instanceIndex;
        root->NodeModel = model;
        return root;
    }else{
        if(model->Hash_ID[instanceIndex] < root->NodeModel->Hash_ID[0]){
            if(root->leftChildInstance == nullptr){
                SceneTreeNode* newNode = new SceneTreeNode();
                newNode->NodeModel = model;
                newNode->instanceCount = instanceIndex;
                newNode->leftChildInstance = nullptr;
                newNode->rightChildInstance = nullptr;
                root->leftChildInstance = newNode;
                return newNode;
            }else{
                return insertInstanceToSceneTree(root->leftChildInstance, model, instanceIndex);
            }
        }else{
            if(root->rightChildInstance == nullptr){
                SceneTreeNode* newNode = new SceneTreeNode();
                newNode->NodeModel = model;
                newNode->instanceCount = instanceIndex;
                newNode->leftChildInstance = nullptr;
                newNode->rightChildInstance = nullptr;
                root->rightChildInstance = newNode;
                return newNode;
            }else{
                return insertInstanceToSceneTree(root->rightChildInstance, model, instanceIndex);
            }
        }
    }
}

void removeInstanceFromSceneTree(SceneTreeNode& root, Model* model, unsigned int instanceIndex){

}

void removeInstanceFromSceneTreeByName(SceneTreeNode& root, Model* model, unsigned int instanceIndex){

}

Model* getInstacesInSceneTree(SceneTreeNode& root, Model* model, unsigned int instanceIndex){
    return nullptr;
}

Model* getInstacesInSceneTreeByName(SceneTreeNode& root, const std::string& name){
    return nullptr;
}
#endif