#include <helpers/sceneTree.hpp>
#include <helpers/octree.hpp>

vector<SceneTreeNode*> hashTable[HASH_TABLE_SIZE];

SceneTreeNode* rootNode = nullptr;
SceneTreeNode* sceneRootNode = nullptr;
SceneTreeNode* selectedNode = nullptr;
SceneTreeNode* previousSelectedNode = nullptr;
vector<SceneTreeNode*> nodesToDrawNormal;
void insertSceneTreeNode(SceneTreeNode *node){
    
    unsigned int slot = node->hashID % HASH_TABLE_SIZE;

    hashTable[slot].push_back(node);
}

void setTreeNode(Model* model, SceneTreeNode* node, SceneTreeNode* parent, Transform transform, string name){
    node->NodeModel = model;
    node->parentNode = parent;
    node->transform = transform;
    node->name = name;

    parent->childrenInstances.push_back(node);


    //get a hash for the node
    std::hash<std::string> str_hash;
    std::hash<float> float_hash;

    size_t h = str_hash(node->name);
    h ^= float_hash(transform.position.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.position.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.position.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.rotation.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(transform.scale.z) + 0x9e3779b9 + (h << 6) + (h >> 2);
    h ^= float_hash(hashTable[h%HASH_TABLE_SIZE].size()) + 0x9e3779b9 + (h << 6) + (h >> 2);

    node->hashID = h;
    parent->childrenHash.push_back(node->hashID);


}

void setTreeNode(Model* model, SceneTreeNode* node, Transform transform, string name, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer, bool renderNormal){
    node->NodeModel = model;

    node->transform = transform;
    node->name = name;
    node->hashID = hashID;
    node->childrenHash = childrenHash;
    node->NodeModel->removeFromDepthBuffer = removeFromDepthBuffer;
    node->NodeModel->renderNormal = renderNormal;
}

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, SceneTreeNode* parent, bool renderNormal) {
    Model* test = new Model(modelPath.c_str(), vertexShader.c_str(), fragmentShader.c_str(), modelName);
    SceneTreeNode *newNode = new SceneTreeNode();

    if (modelName.empty()) {
        delete test;
        delete newNode;
        return;
    }

    test->renderNormal = renderNormal;
    setTreeNode(test, newNode, parent, transform, modelName);
    insertSceneTreeNode(newNode);
    insertNodeIntoOctree(octree, newNode);
}

void createNewModel(const std::string& modelPath, const std::string& vertexShader, const std::string& fragmentShader, const std::string& modelName, Transform transform, unsigned int hashID, vector<unsigned int> childrenHash, bool removeFromDepthBuffer, bool renderNormal) {
    Model* test = new Model(modelPath.c_str(), vertexShader.c_str(), fragmentShader.c_str(), modelName);
    SceneTreeNode *newNode = new SceneTreeNode();

    if (modelName.empty()) {
        delete test;
        delete newNode;
        return;
    }

    setTreeNode(test, newNode, transform, modelName, hashID, childrenHash, removeFromDepthBuffer, renderNormal);
    insertSceneTreeNode(newNode);
}

bool drawSceneNode(SceneTreeNode* node, Camera &camera, glm::mat4 projection, glm::mat4 view){
        /*
    unsigned int instanceVBO;
glGenBuffers(1, &instanceVBO);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations["some indexed position"], GL_STATIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER, 0); 
Then we also need to set its vertex attribute pointer and enable the vertex attribute:


glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
glBindBuffer(GL_ARRAY_BUFFER, 0);	
glVertexAttribDivisor(2, 1);  
*/
    
    if(node == nullptr || node->NodeModel == nullptr){
        return false;
    }

    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, node->transform.position);
    modelMatrix = glm::scale(modelMatrix, node->transform.scale);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(node->transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    bool drawResult = node->NodeModel->Draw(camera, projection, view, modelMatrix, node->transform);
    return drawResult;
}

void removeNodeFromSceneTree(SceneTreeNode* nodeToDelete){
    if(nodeToDelete == nullptr){
        return;
    }

    // Remove the node from its parent's children

    //get the children of the node
    auto &children = nodeToDelete->childrenInstances;
    for(auto &child : children){
        child->parentNode = nodeToDelete->parentNode;
        nodeToDelete->parentNode->childrenInstances.push_back(child);
    }

    if(nodeToDelete->parentNode != nullptr){
        auto &siblings = nodeToDelete->parentNode->childrenInstances;
        siblings.erase(std::remove(siblings.begin(), siblings.end(), nodeToDelete), siblings.end());
        nodeToDelete->parentNode->childrenHash.erase(std::remove(nodeToDelete->parentNode->childrenHash.begin(), nodeToDelete->parentNode->childrenHash.end(), nodeToDelete->hashID), nodeToDelete->parentNode->childrenHash.end());
    }

    // Remove the node from the hash table
    unsigned int slot = nodeToDelete->hashID % HASH_TABLE_SIZE;
    auto &nodes = hashTable[slot];
    nodes.erase(std::remove(nodes.begin(), nodes.end(), nodeToDelete), nodes.end());
    
    deleteNodeFromOctree(octree, nodeToDelete);

    // Delete the node
    delete nodeToDelete;

}

// we are gonna need a getNodeByHash function
SceneTreeNode* getNodeByHash(size_t hashID) {
    unsigned int slot = hashID % HASH_TABLE_SIZE;
    auto &nodes = hashTable[slot];
    for (auto &node : nodes) {
        if (node->hashID == hashID) {
            return node;
        }
    }
    return nullptr;
}
