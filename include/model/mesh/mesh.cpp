#include "mesh.hpp"

void Mesh::Draw(Camera &camera, glm::mat4 modelMatrix, glm::mat4 projection, glm::mat4 viewMatrix)
{


    shader->use();

    shader->setMat4("projection", projection);
    shader->setMat4("view", viewMatrix);
    shader->setMat4("model", modelMatrix);

    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding

        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
            number = to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = to_string(specularNr++);
        else if(name == "texture_normal")
            number = to_string(normalNr++);
        else if(name == "texture_height")
            number = to_string(heightNr++);

 
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        
        shader->setInt((name + number).c_str(), i);

        textures[i].uniformName = (name + number);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::reloadShaders(string vertexShaderPath, string fragmentShaderPath)
{
    delete shader;
    shader = new Shader(vertexShaderPath.c_str(), fragmentShaderPath.c_str());
}

void Mesh::reloadShaders()
{
    reloadShaders(vertexShaderPath, fragmentShaderPath);
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // A great thing about structs is that their memory layout is sequential for all its items.
    // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
    // again translates to 3/2 floats which translates to a byte array.
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, m_BoneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

bool Mesh::isOnFrustum(Frustum &camFrustum, Sphere &sphere)
{
    return (isOnOrForwardPlane(camFrustum.leftFace, sphere) &&
            isOnOrForwardPlane(camFrustum.rightFace, sphere) &&
            isOnOrForwardPlane(camFrustum.topFace, sphere) &&
            isOnOrForwardPlane(camFrustum.bottomFace, sphere) &&
            isOnOrForwardPlane(camFrustum.nearFace, sphere) &&
            isOnOrForwardPlane(camFrustum.farFace, sphere));
}

bool Mesh::isOnOrForwardPlane(Plane &plane, Sphere &sphere)
{
    return plane.getSignedDistanceToPlane(sphere.center) > -sphere.radius;
}