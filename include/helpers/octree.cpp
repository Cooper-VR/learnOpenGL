#include <helpers/octree.hpp>

Frustum octreeCellFrustum;
OctreeNode* octree = nullptr;
vector<OctreeNode*> octreeLeaves;

OctreeNode* generateOctree(){
    //so first we need to see if there is anything in here
    OctreeNode* parent = new OctreeNode();
    parent->position = glm::vec3(0.f, 0.f, 0.f);
    parent->size = glm::vec3(2048.f, 2048.f, 2048.f);

    octreeCellFrustum.topFace = Plane(parent->position + glm::vec3(0.f, parent->size.y / 2, 0.f), glm::vec3(0.f, -1.f, 0.f));
    octreeCellFrustum.bottomFace = Plane(parent->position + glm::vec3(0.f, -parent->size.y / 2, 0.f), glm::vec3(0.f, 1.f, 0.f));
    octreeCellFrustum.leftFace = Plane(parent->position + glm::vec3(-parent->size.x / 2, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
    octreeCellFrustum.rightFace = Plane(parent->position + glm::vec3(parent->size.x / 2, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f));
    octreeCellFrustum.nearFace = Plane(parent->position + glm::vec3(0.f, 0.f, -parent->size.z / 2), glm::vec3(0.f, 0.f, 1.f));
    octreeCellFrustum.farFace = Plane(parent->position + glm::vec3(0.f, 0.f, parent->size.z / 2), glm::vec3(0.f, 0.f, -1.f));

    unsigned int numInCell = 0;
    vector<SceneTreeNode*> nodesInCell;
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        for (int j = 0; j < hashTable[i].size(); j++)
        {
            Model *model = hashTable[i][j]->NodeModel;
            if (model == nullptr)
                continue;

            glm::mat4 centerCircleModel = glm::mat4(1.0f);

            // Correct TRS order: Translate * Rotate * Scale
            centerCircleModel = glm::translate(centerCircleModel, model->transform.position);

            centerCircleModel = glm::rotate(centerCircleModel, glm::radians(model->transform.rotation.z), glm::vec3(0, 0, 1));
            centerCircleModel = glm::rotate(centerCircleModel, glm::radians(model->transform.rotation.y), glm::vec3(0, 1, 0));
            centerCircleModel = glm::rotate(centerCircleModel, glm::radians(model->transform.rotation.x), glm::vec3(1, 0, 0));

            centerCircleModel = glm::scale(centerCircleModel, model->transform.scale);

            // Transform local center to world space
            glm::vec4 worldCenter4 = centerCircleModel * glm::vec4(model->boundingSphere.localCenter, 1.0f);
            model->boundingSphere.center = glm::vec3(worldCenter4);

            // Better radius scaling (handles non-uniform scale reasonably)
            float maxScale = std::max({model->transform.scale.x, model->transform.scale.y, model->transform.scale.z});
            model->boundingSphere.radius = 0.1f * maxScale; // you should cache original radius

            //we need to make a frustum wich will be the octree cell



            bool isOnFrustum = model->isOnFrustum(octreeCellFrustum, model->boundingSphere);

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

    cout << "cell from " << parent->position.x - parent->size.x / 2 << ", " << parent->position.y - parent->size.y / 2 << ", " << parent->position.z - parent->size.z / 2;
    cout << " to " << parent->position.x + parent->size.x / 2 << ", " << parent->position.y + parent->size.y / 2 << ", " << parent->position.z + parent->size.z / 2 << endl;

    cout << "Number of models in cell: " << numInCell << endl;

    return parent;
}

OctreeNode* generateOctree(OctreeNode* parent, vector<SceneTreeNode*> nodesInParent, int depth)
{


    // Build frustum ONCE per node
    Frustum octreeCellFrustum;
    const auto& p = parent->position;
    const auto& s = parent->size;
    octreeCellFrustum.topFace    = Plane(p + glm::vec3(0,  s.y/2, 0), glm::vec3(0, -1, 0));
    octreeCellFrustum.bottomFace = Plane(p + glm::vec3(0, -s.y/2, 0), glm::vec3(0,  1, 0));
    octreeCellFrustum.leftFace   = Plane(p + glm::vec3(-s.x/2, 0, 0), glm::vec3( 1, 0, 0));
    octreeCellFrustum.rightFace  = Plane(p + glm::vec3( s.x/2, 0, 0), glm::vec3(-1, 0, 0));
    octreeCellFrustum.nearFace   = Plane(p + glm::vec3(0, 0, -s.z/2), glm::vec3(0, 0,  1));
    octreeCellFrustum.farFace    = Plane(p + glm::vec3(0, 0,  s.z/2), glm::vec3(0, 0, -1));

    unsigned int numInCell = 0;
    vector<SceneTreeNode*> nodesInCell;

    for (int i = 0; i < nodesInParent.size(); i++)
    {
        SceneTreeNode* stn = nodesInParent[i];

        if (stn == nullptr)
            continue;

        bool isOnFrustum = stn->NodeModel && stn->NodeModel->isOnFrustum(octreeCellFrustum, stn->NodeModel->boundingSphere);
        if (isOnFrustum)
        {
            numInCell++;
            nodesInCell.push_back(stn);
        }
    }

    // Debug print
    
    /*cout << "Depth: " << depth 
         << " | Cell: [" << (p.x - s.x/2) << ", " << (p.y - s.y/2) << ", " << (p.z - s.z/2) 
         << "] -> [" << (p.x + s.x/2) << ", " << (p.y + s.y/2) << ", " << (p.z + s.z/2) 
         << "] | Models: " << numInCell << endl;
*/
    if (numInCell <= 1)
    {
        if (!nodesInCell.empty())
        {
            parent->nodeInCell.push_back(nodesInCell[0]);
        }
        return parent;
    }

    if (depth >= 8)  // max depth
    {
        for (auto* n : nodesInCell)
            parent->nodeInCell.push_back(n);
        return parent;
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

    return parent;
}

int printLeaves(OctreeNode* node){
    if (node == nullptr)
        return 0;

    int modelCount = node->nodeInCell.size(); 
    if (node->nodeInCell.size() > 0)
    {
        octreeLeaves.push_back(node);
        cout << "Leaf node at position: " << node->position.x << ", " << node->position.y << ", " << node->position.z << ", ";
        cout << "Size: " << node->size.x << ", " << node->size.y << ", " << node->size.z << endl;
        for (SceneTreeNode* stn : node->nodeInCell)
        {
            if (stn && stn->NodeModel)
            {
                cout << "  Model: " << stn->name << ", HashID: " << stn->hashID << endl;
            }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        modelCount += printLeaves(node->children[i]);
    }
    return modelCount;
}
