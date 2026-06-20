#ifndef OCTREE_H
#define OCTREE_H

#include <helpers/sceneTree.hpp>
#include <glm/glm.hpp>

using namespace std;

//position is the top left i think
struct OctreeNode{
    OctreeNode* children[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    OctreeNode* parent = nullptr;
    glm::vec3 size = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);
    vector<SceneTreeNode*> nodeInCell; //if this is a leaf node then we will store the model in this cell here, if not then this will be null
} typedef OctreeNode;

extern OctreeNode* octree;
extern vector<OctreeNode*> octreeLeaves;

OctreeNode* generateOctree();

OctreeNode* generateOctree(OctreeNode* parent, vector<SceneTreeNode*> nodesInParent, int depth);

int printLeaves(OctreeNode* node);

#endif