#include <helpers/octree.hpp>

OctreeNode* generateOctree(){
    //so first we need to see if there is anything in here
    OctreeNode* parent = new OctreeNode();
    parent->position = glm::vec3(0.f, 0.f, 0.f);
    parent->size = glm::vec3(1024.f, 1024.f, 1024.f);

    unsigned int numInCell = 0;
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
            model->boundingSphere.radius = 1.0f * maxScale; // you should cache original radius

            //we need to make a frustum wich will be the octree cell
            Frustum octreeCellFrustum;
            octreeCellFrustum.topFace = Plane(parent->position + glm::vec3(0.f, parent->size.y / 2, 0.f), glm::vec3(0.f, -1.f, 0.f));
            octreeCellFrustum.bottomFace = Plane(parent->position + glm::vec3(0.f, -parent->size.y / 2, 0.f), glm::vec3(0.f, 1.f, 0.f));
            octreeCellFrustum.leftFace = Plane(parent->position + glm::vec3(-parent->size.x / 2, 0.f, 0.f), glm::vec3(1.f, 0.f, 0.f));
            octreeCellFrustum.rightFace = Plane(parent->position + glm::vec3(parent->size.x / 2, 0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f));
            octreeCellFrustum.nearFace = Plane(parent->position + glm::vec3(0.f, 0.f, -parent->size.z / 2), glm::vec3(0.f, 0.f, 1.f));
            octreeCellFrustum.farFace = Plane(parent->position + glm::vec3(0.f, 0.f, parent->size.z / 2), glm::vec3(0.f, 0.f, -1.f));


            bool isOnFrustum = model->isOnFrustum(octreeCellFrustum, model->boundingSphere);

            if (isOnFrustum)
            {
                //inside of the octree cell
                numInCell++;
            }
        }
    }

    cout << "cell from " << parent->position.x - parent->size.x / 2 << ", " << parent->position.y - parent->size.y / 2 << ", " << parent->position.z - parent->size.z / 2;
    cout << " to " << parent->position.x + parent->size.x / 2 << ", " << parent->position.y + parent->size.y / 2 << ", " << parent->position.z + parent->size.z / 2 << endl;

    cout << "Number of models in cell: " << numInCell << endl;

    return parent;
}