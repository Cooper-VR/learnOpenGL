
#include <model/model.hpp>

Model::Model(const char *path, const char *vertexShader, const char *fragShader, string name, bool gammaCorrection)
{
    this->gammaCorrection = gammaCorrection;
    //shader = new Shader(vertexShader, fragShader);

    loadModel(path, vertexShader, fragShader);

    std::filesystem::path relativePath(path);
    std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);

    directory = absolutePath.string();
}

Model::Model(const char *path, const char *vertexShader, const char *fragShader, string name, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, bool gammaCorrection)
{
    this->gammaCorrection = gammaCorrection;
    //shader = new Shader(vertexShader, fragShader);

    loadModel(path, vertexShader, fragShader);

    std::filesystem::path relativePath(path);
    std::filesystem::path absolutePath = std::filesystem::absolute(relativePath);

    directory = absolutePath.string();
}

bool Model::Draw( Camera &camera, glm::mat4 projection, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Transform transform){
    glm::mat4 centerCircleModel = glm::mat4(1.0f);

    // Correct TRS order: Translate * Rotate * Scale
    centerCircleModel = glm::translate(centerCircleModel, transform.position);

    centerCircleModel = glm::rotate(centerCircleModel, glm::radians(transform.rotation.z), glm::vec3(0, 0, 1));
    centerCircleModel = glm::rotate(centerCircleModel, glm::radians(transform.rotation.y), glm::vec3(0, 1, 0));
    centerCircleModel = glm::rotate(centerCircleModel, glm::radians(transform.rotation.x), glm::vec3(1, 0, 0));

    centerCircleModel = glm::scale(centerCircleModel, transform.scale);

    // Transform local center to world space
    glm::vec4 worldCenter4 = centerCircleModel * glm::vec4(boundingSphere.localCenter, 1.0f);
    boundingSphere.center = glm::vec3(worldCenter4);

    // Better radius scaling (handles non-uniform scale reasonably)
    float maxScale = std::max({transform.scale.x, transform.scale.y, transform.scale.z});
    boundingSphere.radius = boundingSphere.originalRadius * maxScale; // you should cache original radius

    bool isOnFrustum = this->isOnFrustum(camera.camFrustum, boundingSphere);

    if (!isOnFrustum)
    {
        return false; // Skip drawing this mesh if it's outside the frustum
    }

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        numberOfBatches++;
        numberOfVertices += meshes[i].vertices.size();

        if (removeFromDepthBuffer)
        {
            glDepthMask(GL_FALSE);
        }

        meshes[i].Draw(camera, modelMatrix, projection, viewMatrix);
        if (removeFromDepthBuffer)
        {
            glDepthMask(GL_TRUE);
        }
    }
    return true;
}

void Model::loadModel(string const &path, string vertexShaderPath, string fragmentShaderPath) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    this->directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    long long positionsOfVertices = 0.0;

    processNode(scene->mRootNode, scene, vertexShaderPath, fragmentShaderPath, &positionsOfVertices);

    int numVertices = 0;
    for (const auto& mesh : meshes) {
        numVertices += mesh.vertices.size();
    }

    positionsOfVertices /= numVertices;

    float furthestDistance = 0.0f;
    for (const auto& mesh : meshes) {
        for (const auto& vertex : mesh.vertices) {
            float distance = glm::length(vertex.Position - glm::vec3(positionsOfVertices));
            if (distance > furthestDistance) {
                furthestDistance = distance;
            }
        }
    }

    boundingSphere.center = glm::vec3(positionsOfVertices);
    boundingSphere.radius = furthestDistance;
    boundingSphere.localCenter = boundingSphere.center;
    boundingSphere.originalRadius = boundingSphere.radius;
}

void Model::processNode(aiNode *node, const aiScene *scene, string vertexShaderPath, string fragmentShaderPath, long long *positionsOfVertices)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, vertexShaderPath, fragmentShaderPath, positionsOfVertices));
    }
    for(unsigned int i = 0; i < node->mNumChildren; i++){
        processNode(node->mChildren[i], scene, vertexShaderPath, fragmentShaderPath, positionsOfVertices);
    }
}

void Model::reloadShader(string vertexShaderPath, string fragmentShaderPath) {

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].reloadShaders(vertexShaderPath, fragmentShaderPath);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene, string vertexShaderPath, string fragmentShaderPath, long long *positionsOfVertices){
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

        *positionsOfVertices += vertex.Position.x + vertex.Position.y + vertex.Position.z;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.Normal = vector;

        // can get uv0, uv1, ...
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    //process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++){
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    Shader *shader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

    return Mesh(vertices, indices, textures, shader, vertexShaderPath, fragmentShaderPath);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName){
    vector<Texture> textures;

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++){
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture texture;
        unsigned int id = TextureFromFile(str.C_Str(), this->directory, false);
        if (id != 4294967295)
        {
            texture.id = id;
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
        }
    }

    return textures;
}

unsigned int Model::TextureFromFile(const char *path, const string &directory, bool gamma){
    
    //cout << path << endl;
    string filename = string(path);
    filename = directory + '/' + filename;
    filename = path;

    //cout << filename << endl;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 4294967295;
    }

    return textureID;
}

bool Model::isOnFrustum(Frustum &camFrustum, Sphere &sphere)
{
    return (isOnOrForwardPlane(camFrustum.leftFace, sphere) &&
            isOnOrForwardPlane(camFrustum.rightFace, sphere) &&
            isOnOrForwardPlane(camFrustum.topFace, sphere) &&
            isOnOrForwardPlane(camFrustum.bottomFace, sphere) &&
            isOnOrForwardPlane(camFrustum.nearFace, sphere) &&
            isOnOrForwardPlane(camFrustum.farFace, sphere));
}

bool Model::isOnOrForwardPlane(Plane &plane, Sphere &sphere)
{
    return plane.getSignedDistanceToPlane(sphere.center) > -sphere.radius;
}
