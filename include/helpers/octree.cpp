#include <helpers/octree.hpp>


OctreeNode* octree = nullptr;
vector<OctreeNode*> octreeLeaves;

OctreeNode* generateOctree(){
    //so first we need to see if there is anything in here
    OctreeNode* parent = new OctreeNode();
    parent->position = glm::vec3(0.f, 0.f, 0.f);
    parent->size = glm::vec3(2048.f, 2048.f, 2048.f);

    Frustum octreeCellFrustum;
    octreeCellFrustum.topFace = Plane(parent->position + glm::vec3(0.f, parent->size.y / 2, 0.f), glm::vec3(0.f, -1.f, 0.f));
    octreeCellFrustum.bottomFace = Plane(parent->position + glm::vec3(0.f, -parent->size.y / 2, 0.f), glm::vec3(0.f, 1.f, 0.f));
    octreeCellFrustum.leftFace = Plane(parent->position + glm::vec3(-parent->size.x / 2, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
    octreeCellFrustum.rightFace = Plane(parent->position + glm::vec3(parent->size.x / 2, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f));
    octreeCellFrustum.nearFace = Plane(parent->position + glm::vec3(0.f, 0.f, -parent->size.z / 2), glm::vec3(0.f, 0.f, 1.f));
    octreeCellFrustum.farFace = Plane(parent->position + glm::vec3(0.f, 0.f, parent->size.z / 2), glm::vec3(0.f, 0.f, -1.f));

    parent->octreeCellFrustum = octreeCellFrustum;

    unsigned int numInCell = 0;
    vector<SceneTreeNode*> nodesInCell;
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        for (int j = 0; j < hashTable[i].size(); j++)
        {
            SceneTreeNode *model = hashTable[i][j];
            if (model == nullptr || model->NodeModel == nullptr)
                continue;

            glm::mat4 modelMatrix = glm::mat4(1.0f);

            // Standard TRS order (Translate * Rotate * Scale)
            modelMatrix = glm::translate(modelMatrix, model->transform.position);

            modelMatrix = glm::rotate(modelMatrix, glm::radians(model->transform.rotation.z), glm::vec3(0, 0, 1));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(model->transform.rotation.y), glm::vec3(0, 1, 0));
            modelMatrix = glm::rotate(modelMatrix, glm::radians(model->transform.rotation.x), glm::vec3(1, 0, 0));

            modelMatrix = glm::scale(modelMatrix, model->transform.scale);

            // Transform local center to world space
            glm::vec4 worldCenter4 = modelMatrix * glm::vec4(model->NodeModel->boundingSphere.localCenter, 1.0f);
            model->NodeModel->boundingSphere.center = glm::vec3(worldCenter4);

            // Better radius scaling (handles non-uniform scale reasonably)
            float maxScale = std::max({model->transform.scale.x, model->transform.scale.y, model->transform.scale.z});
            model->NodeModel->boundingSphere.radius = 1.0f * maxScale; // you should cache original radius

            //we need to make a frustum wich will be the octree cell

            bool isOnFrustum = model->NodeModel->isOnFrustum(parent->octreeCellFrustum, model->NodeModel->boundingSphere);

            if (isOnFrustum)
            {
                //inside of the octree cell
                numInCell++;
                nodesInCell.push_back(hashTable[i][j]);
            }
        }
    }
    
    //ok now if there are more than 1 models in the cell we need to make 8 children and run this again 
    //but with the children as the parent, passing a list of models inside so that the list will get smaller as it gets deeper
    //if we have 1 or 0 models in the cell then we can stop, we will store the list of models in this leaf and return

    if (numInCell <= 1){
        //we are at a leaf node, we can stop
        if (!nodesInCell.empty())
            parent->nodeInCell.push_back(nodesInCell[0]);
        parent->isLeaf = true;
        return parent;
    }else{
        //we need to make 8 children and run this again 
        //but with the children as the parent, passing a list of models inside so that the list will get smaller as it gets deeper
        for (int i = 0; i < 8; i++)
        {
            parent->children[i] = new OctreeNode();
            parent->children[i]->parent = parent;
            parent->children[i]->size = parent->size / 2.f;

            //calculate position of child
            parent->children[i]->position = parent->position;
            if (i & 1) // right half
                parent->children[i]->position.x += parent->size.x / 4.f;
            else // left half
                parent->children[i]->position.x -= parent->size.x / 4.f;

            if (i & 2) // top half
                parent->children[i]->position.y += parent->size.y / 4.f;
            else // bottom half
                parent->children[i]->position.y -= parent->size.y / 4.f;

            if (i & 4) // far half
                parent->children[i]->position.z += parent->size.z / 4.f;
            else // near half
                parent->children[i]->position.z -= parent->size.z / 4.f;

            //we need to run this function again with the child as the new parent and the list of models in this cell as the new list to check against
            generateOctree(parent->children[i], nodesInCell, 1);
        }
    }

    parent->isLeaf = false; //this node is not a leaf since it has children
    return parent;
}

void generateOctree(OctreeNode* parent, vector<SceneTreeNode*> nodesInParent, int depth)
{
    Frustum octreeCellFrustum;
    octreeCellFrustum.topFace = Plane(parent->position + glm::vec3(0.f, parent->size.y / 2, 0.f), glm::vec3(0.f, -1.f, 0.f));
    octreeCellFrustum.bottomFace = Plane(parent->position + glm::vec3(0.f, -parent->size.y / 2, 0.f), glm::vec3(0.f, 1.f, 0.f));
    octreeCellFrustum.leftFace = Plane(parent->position + glm::vec3(-parent->size.x / 2, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
    octreeCellFrustum.rightFace = Plane(parent->position + glm::vec3(parent->size.x / 2, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f));
    octreeCellFrustum.nearFace = Plane(parent->position + glm::vec3(0.f, 0.f, -parent->size.z / 2), glm::vec3(0.f, 0.f, 1.f));
    octreeCellFrustum.farFace = Plane(parent->position + glm::vec3(0.f, 0.f, parent->size.z / 2), glm::vec3(0.f, 0.f, -1.f));

    parent->octreeCellFrustum = octreeCellFrustum;

    parent->depth = depth;

    unsigned int numInCell = 0;
    vector<SceneTreeNode*> nodesInCell;

    for (int i = 0; i < nodesInParent.size(); i++)
    {
        SceneTreeNode* stn = nodesInParent[i];

        if (stn == nullptr)
            continue;

        bool isOnFrustum = stn->NodeModel->isOnFrustum(parent->octreeCellFrustum, stn->NodeModel->boundingSphere);
        if (isOnFrustum)
        {
            auto *bs = &stn->NodeModel->boundingSphere;
            numInCell++;
            nodesInCell.push_back(stn);
        }
    }

    // Debug print

    if (numInCell <= 1)
    {
        if (!nodesInCell.empty())
        {
            parent->nodeInCell.push_back(nodesInCell[0]);
            octreeLeaves.push_back(parent);
            parent->isLeaf = true;
        }

        return;
    }

    if (depth >= 8)  // max depth
    {
        for (auto* n : nodesInCell){
            parent->nodeInCell.push_back(n);
        }
        parent->isLeaf = true;
        octreeLeaves.push_back(parent);
        return;
    }

    // Subdivide
    for (int i = 0; i < 8; ++i)
    {
        parent->children[i] = new OctreeNode();
        auto* child = parent->children[i];
        child->parent = parent;
        child->size = parent->size / 2.0f;

        child->position = parent->position;
        if (i & 1) child->position.x += parent->size.x / 4.0f;
        else       child->position.x -= parent->size.x / 4.0f;
        if (i & 2) child->position.y += parent->size.y / 4.0f;
        else       child->position.y -= parent->size.y / 4.0f;
        if (i & 4) child->position.z += parent->size.z / 4.0f;
        else       child->position.z -= parent->size.z / 4.0f;

        generateOctree(child, nodesInCell, depth + 1);
    }

    parent->isLeaf = false;  // This node is not a leaf since it has children
    return;
}

int printLeaves(){
    for (OctreeNode* leaf : octreeLeaves)
    {
        cout << "Leaf node at position: " << leaf->position.x << ", " << leaf->position.y << ", " << leaf->position.z;
        cout << " with size: " << leaf->size.x << ", " << leaf->size.y << ", " << leaf->size.z;
        cout << " contains " << leaf->nodeInCell.size() << " models." << endl;
        for (SceneTreeNode* stn : leaf->nodeInCell)
        {
            if (stn && stn->NodeModel)
            {
                cout << "    Model in leaf: " << stn->name << ", HashID: " << stn->hashID << endl;
            }
        }
    }

    return octreeLeaves.size();
}

void deleteObjectFromOctree(OctreeNode* node, SceneTreeNode* stn){
    //ima sorta copy the insert, just keeping track of the nodes i would insert into and removing from those nodes instead of adding to them
    if (node == nullptr || stn == nullptr || stn->NodeModel == nullptr)
    {
        cout << "Error: Null node or model passed to deleteObjectFromOctree." << endl;
        return;
    }

    bool isOnFrustum = stn->NodeModel->isOnFrustum(node->octreeCellFrustum, stn->NodeModel->boundingSphere);

    if (!isOnFrustum)
        return; //model is not in this cell
    
    if (node->isLeaf){
        //we are at a leaf node, we can stop
        auto it = std::find(node->nodeInCell.begin(), node->nodeInCell.end(), stn);
        if (it != node->nodeInCell.end())
        {
            cout << "Deleting model " << stn->name << " from octree leaf at position: " << node->position.x << ", " << node->position.y << ", " << node->position.z << endl;
            node->nodeInCell.erase(it);
            if (node->nodeInCell.empty())
            {
                node->isLeaf = false;
                auto leafIt = std::find(octreeLeaves.begin(), octreeLeaves.end(), node);
                if (leafIt != octreeLeaves.end())
                {
                    octreeLeaves.erase(leafIt);
                }

                //lets check the sibling nodes too, if they are empty then the children can be deleted and the parent can become an empty leaf
                CheckSiblings(node);
            }
        }
        return;
    }

}

void deleteOctree(OctreeNode* node){
    if (node == nullptr)
        return;

    for (int i = 0; i < 8; ++i)
    {
        deleteOctree(node->children[i]);
        node->children[i] = nullptr;
    }

    delete node;
}

void CheckSiblings(OctreeNode* node){
    if (node == nullptr || node->parent == nullptr)
        return;
    OctreeNode* parent = node->parent;
    if (parent == nullptr)
        return;

    bool allSiblingsEmpty = true;

    for (int i = 0; i < 8; ++i)
    {
        if (parent->children[i] != nullptr && !parent->children[i]->nodeInCell.empty())
        {
            allSiblingsEmpty = false;
            break;
        }
    }

    if (allSiblingsEmpty)
    {
        // Delete all children
        for (int i = 0; i < 8; ++i)
        {
            delete parent->children[i];
            parent->children[i] = nullptr;
        }

        //check the parent now, a sorta waterfall effect
        CheckSiblings(parent);
    }
}

void insertObjectIntoOctree(OctreeNode* root, SceneTreeNode* stn){
    if (root == nullptr || stn == nullptr || stn->NodeModel == nullptr)
    {
        cout << "Error: Null node or model passed to insertObjectIntoOctree." << endl;
        return;
    }

    //we need to check if the model is in the octree cell, if it is then we need to insert it into the correct child node
    bool isOnFrustum = stn->NodeModel->isOnFrustum(root->octreeCellFrustum, stn->NodeModel->boundingSphere);

    if (!isOnFrustum)
        return; //model is not in this cell

    //if we are the only one in here and no children then add here
    if (root->isLeaf){
        //we are at a leaf node, we can stop
        root->nodeInCell.push_back(stn);
        return;
    }

    if (root->children[0] == nullptr && !root->isLeaf){
        //has no children so its an empty leaf
        root->nodeInCell.push_back(stn);
        root->isLeaf = true;
        octreeLeaves.push_back(root);
        return;
    }

    //we are not at a leaf node, we need to check which child node this model belongs in and insert it there

    for (int i = 0; i < 8; i++)
    {
        insertObjectIntoOctree(root->children[i], stn);
    }
}
