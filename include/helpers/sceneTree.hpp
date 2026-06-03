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

void removeInstanceFromSceneTree(SceneTreeNode& root, Model* model, unsigned int instanceIndex){}

void removeInstanceFromSceneTreeByName(SceneTreeNode* root, Model* model, string name){
    // Find the node by matching model pointer and instance name
    SceneTreeNode* found = nullptr;
    SceneTreeNode* parent = nullptr;

    std::function<void(SceneTreeNode*, SceneTreeNode*)> findNode = [&](SceneTreeNode* node, SceneTreeNode* par){
        if(!node || found) return;
        if(node->NodeModel == model){
            unsigned int idx = node->instanceCount;
            if(idx < model->names.size() && model->names[idx] == name){
                found = node;
                parent = par;
                return;
            }
        }
        findNode(node->leftChildInstance, node);
        findNode(node->rightChildInstance, node);
    };

    findNode(root, nullptr);
    if(!found) return; // not present

    // Collect nodes from a subtree to reinsert later
    vector<pair<Model*, unsigned int>> toReinsert;
    std::function<void(SceneTreeNode*)> collect = [&](SceneTreeNode* n){
        if(!n) return;
        collect(n->leftChildInstance);
        // only reinsert valid entries
        if(n->NodeModel) toReinsert.push_back({n->NodeModel, n->instanceCount});
        collect(n->rightChildInstance);
    };

    std::function<void(SceneTreeNode*)> deleteSubtree = [&](SceneTreeNode* n){
        if(!n) return;
        deleteSubtree(n->leftChildInstance);
        deleteSubtree(n->rightChildInstance);
        delete n;
    };

    // If found is root (no parent), clear root's model and reinsert children
    if(parent == nullptr){
        SceneTreeNode* left = root->leftChildInstance;
        SceneTreeNode* right = root->rightChildInstance;

        root->leftChildInstance = nullptr;
        root->rightChildInstance = nullptr;

        collect(left);
        collect(right);

        deleteSubtree(left);
        deleteSubtree(right);

        root->NodeModel = nullptr;
        root->instanceCount = 0;

        for(auto &p : toReinsert){
            insertInstanceToSceneTree(root, p.first, p.second);
        }
        return;
    }

    // Detach found from parent
    if(parent->leftChildInstance == found) parent->leftChildInstance = nullptr;
    else if(parent->rightChildInstance == found) parent->rightChildInstance = nullptr;

    // Collect child nodes of the found subtree (excluding found itself)
    collect(found->leftChildInstance);
    collect(found->rightChildInstance);

    // Delete the found subtree
    deleteSubtree(found);

    // Reinsert collected nodes
    for(auto &p : toReinsert){
        insertInstanceToSceneTree(root, p.first, p.second);
    }
}

Model* getInstacesInSceneTree(SceneTreeNode& root, Model* model, unsigned int instanceIndex){return nullptr;}

Model* getInstacesInSceneTreeByName(SceneTreeNode& root, const std::string& name){return nullptr;}
#endif