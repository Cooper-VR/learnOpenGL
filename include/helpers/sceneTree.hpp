#ifndef SCENE_TREE_HPP
#define SCENE_TREE_HPP

#include <vector>
#include <utility>
#include <model/model.hpp>

struct SceneTreeNode{
    Model* NodeModel;
    unsigned int instanceCount;
    SceneTreeNode* leftChildInstance;
    SceneTreeNode* rightChildInstance;

    vector<SceneTreeNode*> childrenInstances;
    SceneTreeNode* parentNode;

} typedef SceneTreeNode;

static inline size_t getSceneTreeNodeHash(SceneTreeNode* node){
    if(node == nullptr || node->NodeModel == nullptr || node->instanceCount >= node->NodeModel->Hash_ID.size()){
        return 0;
    }

    return node->NodeModel->Hash_ID[node->instanceCount];
}

static inline void setSceneTreeChildParent(SceneTreeNode* child, SceneTreeNode* parent){
    if(child != nullptr){
        child->parentNode = parent;
    }
}

static inline SceneTreeNode* removeSceneTreeNodeByHash(SceneTreeNode* node, size_t ID){
    if(node == nullptr || node->NodeModel == nullptr){
        return nullptr;
    }

    size_t currentHash = getSceneTreeNodeHash(node);
    if(ID < currentHash){
        node->leftChildInstance = removeSceneTreeNodeByHash(node->leftChildInstance, ID);
        setSceneTreeChildParent(node->leftChildInstance, node);
        return node;
    }

    if(ID > currentHash){
        node->rightChildInstance = removeSceneTreeNodeByHash(node->rightChildInstance, ID);
        setSceneTreeChildParent(node->rightChildInstance, node);
        return node;
    }

    if(node->leftChildInstance == nullptr){
        SceneTreeNode* replacement = node->rightChildInstance;
        setSceneTreeChildParent(replacement, node->parentNode);
        delete node;
        return replacement;
    }

    if(node->rightChildInstance == nullptr){
        SceneTreeNode* replacement = node->leftChildInstance;
        setSceneTreeChildParent(replacement, node->parentNode);
        delete node;
        return replacement;
    }

    SceneTreeNode* successorParent = node;
    SceneTreeNode* successor = node->rightChildInstance;
    while(successor->leftChildInstance != nullptr){
        successorParent = successor;
        successor = successor->leftChildInstance;
    }

    node->NodeModel = successor->NodeModel;
    node->instanceCount = successor->instanceCount;

    SceneTreeNode* successorReplacement = successor->rightChildInstance;
    setSceneTreeChildParent(successorReplacement, successorParent);

    if(successorParent->leftChildInstance == successor){
        successorParent->leftChildInstance = successorReplacement;
    }else{
        successorParent->rightChildInstance = successorReplacement;
    }

    delete successor;
    return node;
}

static inline void syncSceneTreeInstanceIndices(SceneTreeNode* node, unsigned int removedIndex){
    if(node == nullptr || node->NodeModel == nullptr){
        return;
    }

    if(node->instanceCount > removedIndex){
        node->instanceCount--;
    }

    syncSceneTreeInstanceIndices(node->leftChildInstance, removedIndex);
    syncSceneTreeInstanceIndices(node->rightChildInstance, removedIndex);
}

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

void removeInstanceFromSceneTree(SceneTreeNode* root, Model* model, size_t ID){
    if(root == nullptr){
        return;
    }

    SceneTreeNode* updatedRoot = removeSceneTreeNodeByHash(root, ID);
    if(updatedRoot == nullptr){
        root->NodeModel = nullptr;
        root->instanceCount = 0;
        root->leftChildInstance = nullptr;
        root->rightChildInstance = nullptr;
        root->childrenInstances.clear();
        root->parentNode = nullptr;
        return;
    }

    if(updatedRoot != root){
        root->NodeModel = updatedRoot->NodeModel;
        root->instanceCount = updatedRoot->instanceCount;
        root->leftChildInstance = updatedRoot->leftChildInstance;
        root->rightChildInstance = updatedRoot->rightChildInstance;
        root->childrenInstances = updatedRoot->childrenInstances;
        root->parentNode = updatedRoot->parentNode;
        setSceneTreeChildParent(root->leftChildInstance, root);
        setSceneTreeChildParent(root->rightChildInstance, root);
        delete updatedRoot;
    }
}

void removeInstanceFromSceneTreeByName(SceneTreeNode* root, Model* model, string name){
    for(int i = 0; i < model->instanceCount; i++){
        if(model->names[i] == name){
            cout << "removing instance with name: " << name << endl;
            unsigned int removedIndex = i;
            size_t instanceHash = model->Hash_ID[i];
            removeInstanceFromSceneTree(root, model, instanceHash);
            model->removeInstance(instanceHash);
            syncSceneTreeInstanceIndices(root, removedIndex);
            cout << "instance removed from scene tree" << endl;
            return;
        }
    }
}

Model* getInstacesInSceneTree(SceneTreeNode& root, Model* model, unsigned int instanceIndex){return nullptr;}

Model* getInstacesInSceneTreeByName(SceneTreeNode& root, const std::string& name){return nullptr;}
#endif