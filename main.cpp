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
    GLFWwindow* window = setupOpenGL();
    if (window == nullptr)
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    setUpImGui(window);
    loadData(window);
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


        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.001f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
        glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        vector<SceneTreeNode*> nodesToDrawLast;
        vector<OctreeNode*> culledNodes;
        if (octree != nullptr)
        {
            culledNodes = getCulledOctreeNodes(octree, camera.camFrustum);
            numberOfCells = culledNodes.size();
        }

        if (octree != nullptr)
        {
            for (int i = 0; i < culledNodes.size(); i++)
            {
                for (int j = 0; j < culledNodes[i]->nodeInCell.size(); j++)
                {
                    SceneTreeNode *stn = culledNodes[i]->nodeInCell[j];
                    if (stn->NodeModel == nullptr)
                        continue;
                    Model *model = stn->NodeModel;
                    if (model == nullptr)
                        continue;
                    if (stn->frameID == frameID)
                        continue;


                    stn->frameID = frameID;
                    bool drawResult = drawSceneNode(stn, camera, projection, view);

                    if (drawResult)
                    {

                        numberOfVertices += model->numberOfVertices;
                        numberOfBatches += model->numberOfBatches;
                        model->numberOfVertices = 0;
                        model->numberOfBatches = 0;
                    }

                    if (drawResult)
                    {
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

                            model->meshes[k].shader->setVec3("direction", dirLightDirection[0], dirLightDirection[1], dirLightDirection[2]);
                            model->meshes[k].shader->setVec3("ambient", dirLightAmbientColor[0], dirLightAmbientColor[1], dirLightAmbientColor[2]);
                            model->meshes[k].shader->setVec3("diffuse", dirLightDiffuseColor[0], dirLightDiffuseColor[1], dirLightDiffuseColor[2]);
                            model->meshes[k].shader->setVec3("specular", dirLightSpecularColor[0], dirLightSpecularColor[1], dirLightSpecularColor[2]);
                        }
                    }
                }
            }

            for (int i = 0; i < nodesToDrawNormal.size(); i++)
            {
                if (nodesToDrawNormal[i]->NodeModel->removeFromDepthBuffer)
                {
                    nodesToDrawLast.push_back(nodesToDrawNormal[i]);
                    continue;
                }
                drawSceneNode(nodesToDrawNormal[i], camera, projection, view);
            }

            // draw nodes that should be removed from depth buffer last so that they are on top of everything else and not affected by depth testing
            for (int i = 0; i < nodesToDrawLast.size(); i++)
            {
                drawSceneNode(nodesToDrawLast[i], camera, projection, view);
            }
        }
        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
        // clear all relevant buffers
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
        glClear(GL_COLOR_BUFFER_BIT);

        screenShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthTexture);

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        drawAllUI(window);


        glfwSwapBuffers(window);
        glfwPollEvents();
        numberOfVertices = 0;
        numberOfBatches = 0;

        frameID++;
    }

    cout << "closing application" << endl;

    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &framebuffer);

    for (unsigned int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        for (unsigned int j = 0; j < hashTable[i].size(); j++)
        {
            delete hashTable[i][j];
        }
    }

    deleteOctree(octree);

    saveData(window);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}