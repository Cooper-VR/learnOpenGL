#ifndef OCTREE_H
#define OCTREE_H

#include <glm/glm.hpp>
#include <helpers/sceneTree.hpp>

struct OctreeNode{
    OctreeNode* children[8];
    OctreeNode* parent;
    glm::vec3 size;
    glm::vec3 position;
};

OctreeNode* generateOctree(OctreeNode* parent){
    //so first we need to see if there is anything in here

        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                
            }
        }

}

#endif