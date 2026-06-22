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
    unsigned int depth = 0;
    vector<SceneTreeNode*> nodeInCell; //if this is a leaf node then we will store the model in this cell here, if not then this will be null
    Frustum octreeCellFrustum; //the frustum of this cell, used to check if a model is in this cell or not

    bool intersectsFrustum(Frustum camFrustum) const
    {
        // Octree cell is an AABB: center = position, extents = size * 0.5f
        const glm::vec3 extents = size * 0.5f;
        const glm::vec3 center   = position;

        cout << "Checking octree cell at position: (" << position.x << ", " << position.y << ", " << position.z
             << ") with size: (" << size.x << ", " << size.y << ", " << size.z
             << ") against camera frustum.\n";
        // Test against all 6 planes
        return isAABBOnOrForwardPlane(camFrustum.leftFace,   center, extents) &&
               isAABBOnOrForwardPlane(camFrustum.rightFace,  center, extents) &&
               isAABBOnOrForwardPlane(camFrustum.topFace,    center, extents) &&
               isAABBOnOrForwardPlane(camFrustum.bottomFace, center, extents) &&
               isAABBOnOrForwardPlane(camFrustum.nearFace,   center, extents) &&
               isAABBOnOrForwardPlane(camFrustum.farFace,    center, extents);
    }

    private:
        static bool isAABBOnOrForwardPlane(Plane plane, glm::vec3 center, glm::vec3 extents)
        {

            // Same logic you already had, just made static/helper
            const float r = extents.x * std::abs(plane.normal.x) +
                extents.y * std::abs(plane.normal.y) +
                extents.z * std::abs(plane.normal.z);

            float signedDist = plane.getSignedDistanceToPlane(center);

            // For debugging - remove later
            cout << "    Plane normal: (" << plane.normal.x << ", " << plane.normal.y << ", " << plane.normal.z 
                << ") dist: " << plane.distance 
                << " signedDist: " << signedDist << " r: " << r << "\n";

            return signedDist >= -r;   // Box intersects or is on the inside of the plane
        }
} typedef OctreeNode;

extern OctreeNode* octree;
extern vector<OctreeNode*> octreeLeaves;

OctreeNode* generateOctree();

void generateOctree(OctreeNode* parent, vector<SceneTreeNode*> nodesInParent, int depth);

void insertNodeIntoOctree(OctreeNode* node, SceneTreeNode* stn);

void deleteNodeFromOctree(OctreeNode* node, SceneTreeNode* stn);

void deleteOctree(OctreeNode* node);

void updateNodeInOctree(OctreeNode* node, SceneTreeNode* stn);

int printLeaves();

#endif