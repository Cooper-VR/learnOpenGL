#ifndef OCTREE_H
#define OCTREE_H

#include <helpers/sceneTree.hpp>
#include <glm/glm.hpp>

//position is the top left i think
struct OctreeNode{
    OctreeNode* children[8];
    OctreeNode* parent;
    glm::vec3 size;
    glm::vec3 position;
} typedef OctreeNode;

OctreeNode* generateOctree();

#endif