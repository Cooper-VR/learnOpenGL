#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <stack>
#include <shaders/shader.hpp>

#include <helpers/ui.hpp>
#include <helpers/sceneTree.hpp>
#include <helpers/setup.hpp>

using namespace std;
namespace fs = std::filesystem;

int main()
{
    loadData();
    GLFWwindow* window = setupOpenGL();
    if (window == nullptr)
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    setUpImGui(window);

    sceneRootNode = new SceneTreeNode();
    sceneRootNode->name = "sceneRoot";
    sceneRootNode->transform = Transform{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)};
    sceneRootNode->hashID = 0;
    sceneRootNode->childrenInstances = {};
    insertSceneTreeNode(sceneRootNode);

    string path = "resources/models/testCube.fbx";
    string fragment = "resources/shaders/litObject_fragment.glsl";
    string vertex = "resources/shaders/litObject_vertex.glsl";

    while (!glfwWindowShouldClose(window))
    {
        // Skip frame if minimized
        if (SCR_WIDTH == 0 || SCR_HEIGHT == 0)
        {
            SCR_HEIGHT = 1;
            SCR_WIDTH = 1;
        }

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        {

            if (!mousePressLeft)
            {
                glm::vec3 newPos = camera.Position;
                newPos += camera.Front;
            }
            mousePressLeft = true;
        }
        else
        {
            mousePressLeft = false;
        }


        glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 projection = glm::perspective(glm::radians(cameraFOV), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                Model *model = hashTable[i][j]->NodeModel;
                if (model == nullptr)
                    continue;
                numberOfVertices += model->numberOfVertices;
                numberOfBatches += model->numberOfBatches;
                model->numberOfVertices = 0;
                model->numberOfBatches = 0;

                for (int k = 0; k < model->meshes.size(); k++)
                {
                    model->meshes[k].shader->use();
                    model->meshes[k].shader->setVec3("viewPos", camera.Position);
                    // set view pos for shader editor part too
                    int vertexVec3Counter = 0;
                    for (int j = 0; j < vertexUniforms.size(); j++)
                    {
                        if (vertexUniforms[j] == "viewPos")
                        {
                            vertexUniformVec3s[vertexVec3Counter] = camera.Position;
                            model->meshes[k].shader->setVec3(vertexUniforms[j].c_str(), vertexUniformVec3s[vertexVec3Counter]);
                        }

                        if (vertexUniformTypes[j] == "vec3")
                        {
                            vertexVec3Counter++;
                        }
                    }

                    int fragmentVec3Counter = 0;
                    for (int j = 0; j < fragmentUniforms.size(); j++)
                    {
                        if (fragmentUniforms[j] == "viewPos")
                        {
                            fragmentUniformVec3s[fragmentVec3Counter] = camera.Position;
                            model->meshes[k].shader->setVec3(fragmentUniforms[j].c_str(), fragmentUniformVec3s[fragmentVec3Counter]);
                        }

                        if (fragmentUniformTypes[j] == "vec3")
                        {
                            fragmentVec3Counter++;
                        }
                    }

                    model->meshes[k].shader->setVec3("dirLight.direction", dirLightDirection[0], dirLightDirection[1], dirLightDirection[2]);
                    model->meshes[k].shader->setVec3("dirLight.ambient", dirLightAmbientColor[0], dirLightAmbientColor[1], dirLightAmbientColor[2]);
                    model->meshes[k].shader->setVec3("dirLight.diffuse", dirLightDiffuseColor[0], dirLightDiffuseColor[1], dirLightDiffuseColor[2]);
                    model->meshes[k].shader->setVec3("dirLight.specular", dirLightSpecularColor[0], dirLightSpecularColor[1], dirLightSpecularColor[2]);

                    if (runtimeTextureTargetNode && runtimeTextureTargetNode->NodeModel == model && runtimeTestTextureID != 0)
                    {
                        glActiveTexture(GL_TEXTURE0 + runtimeTestTextureUnit);
                        glBindTexture(GL_TEXTURE_2D, runtimeTestTextureID);
                        model->meshes[k].shader->setInt("testTexture", runtimeTestTextureUnit);
                    }
                }

                drawSceneNode(hashTable[i][j], projection, view);
            }
        }

        drawAllUI();



        glfwSwapBuffers(window);
        glfwPollEvents();
        numberOfVertices = 0;
        numberOfBatches = 0;
    }

    cout << "closing application" << endl;

    for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        for (unsigned int j = 0; j < hashTable[i].size(); j++)
        {
            delete hashTable[i][j];
        }
    }

    saveData();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}