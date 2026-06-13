#ifndef ui_hpp
#define ui_hpp

#include <sys/stat.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui/imgui.h>
#include <model/model.hpp>
#include "helpers/sceneTree.hpp"
#include "helpers/setup.hpp"

using namespace std;
namespace fs = std::filesystem;

static int numberOfVertices = 0;
static int numberOfBatches = 0;

float skyColor[3]{0.4f, 0.4f, 0.9f};
float dirLightDirection[3] = {-0.2f, -1.0f, -0.3f};
float dirLightDiffuseColor[3] = {0.5f, 0.5f, 0.5f};
float dirLightAmbientColor[3] = {0.2f, 0.2f, 0.2f};
float dirLightSpecularColor[3] = {1.0f, 1.0f, 1.0f};
float newVal = 0.0f;
int newIntVal = 0;
float newFloat3Val[3] = {0.0f, 0.0f, 0.0f};
char newStringVal[256] = "";

static fs::path currentPath = fs::current_path();
static std::string selectedFile = "";

// for shader editing
bool alreadyCreated = false;

string vertexShaderName;
string fragmentShaderName;

vector<string> vertexUniforms;
vector<string> fragmentUniforms;

vector<string> vertexUniformTypes;
vector<string> fragmentUniformTypes;

vector<int> vertexUniformInts;
vector<int> fragmentUniformInts;

vector<float> vertexUniformFloats;
vector<float> fragmentUniformFloats;

vector<glm::vec3> vertexUniformVec3s;
vector<glm::vec3> fragmentUniformVec3s;

vector<string> textureNames;
vector<string> texturePaths;
vector<int> selectedVertexTextureIDs;
vector<int> selectedFragmentTextureIDs;

char selectedTextureName[256] = "";
vector<string> selectedTextureNames;
string texturePath("resources/textures");

char selectedName[256] = "";
vector<string> shaderNames;
string path("resources/shaders");
static int intCounter = 0;
static int floatCounter = 0;
static int vec3Counter = 0;
static int textureIDCounter = 0;

float depthLinearizationNear = 0.1f;
float depthLinearizationFar = 100.0f;
float fogColor[3] = {0.5f, 0.5f, 0.5f};

static int selected = -1;
static int previousSelected = -1;

void saveScene()
{
    // save the scene to a file
    ofstream sceneFile;
    sceneFile.open("localData/scene.sn");
    if (sceneFile.is_open())
    {
        //first print the root node stuff

        SceneTreeNode* root = sceneRootNode;

        for (int i = 0; i < HASH_TABLE_SIZE; i++){
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                if (sceneRootNode != hashTable[i][j])
                {
                    sceneFile << hashTable[i][j]->name << endl;
                    sceneFile << hashTable[i][j]->hashID << endl;
                    sceneFile << hashTable[i][j]->NodeModel->directory << endl;
                    sceneFile << hashTable[i][j]->transform.position.x << ' ' << hashTable[i][j]->transform.position.y << ' ' << hashTable[i][j]->transform.position.z << endl;
                    sceneFile << hashTable[i][j]->transform.rotation.x << ' ' << hashTable[i][j]->transform.rotation.y << ' ' << hashTable[i][j]->transform.rotation.z << endl;
                    sceneFile << hashTable[i][j]->transform.scale.x << ' ' << hashTable[i][j]->transform.scale.y << ' ' << hashTable[i][j]->transform.scale.z << endl;
                    sceneFile << hashTable[i][j]->childrenInstances.size() << endl;
                    sceneFile << hashTable[i][j]->NodeModel->removeFromDepthBuffer << endl;
                    for (int k = 0; k < hashTable[i][j]->childrenInstances.size(); k++)
                    {
                        sceneFile << hashTable[i][j]->childrenInstances[k]->hashID << endl;
                    }
                    sceneFile << hashTable[i][j]->NodeModel->meshes.size() << endl;
                    for (int k = 0; k < hashTable[i][j]->NodeModel->meshes.size(); k++)
                    {
                        sceneFile << hashTable[i][j]->NodeModel->meshes[k].fragmentShaderPath << endl;
                        sceneFile << hashTable[i][j]->NodeModel->meshes[k].vertexShaderPath << endl;

                        ifstream vertexShaderFile;
                        vertexShaderFile.open(hashTable[i][j]->NodeModel->meshes[k].vertexShaderPath);
                        if (vertexShaderFile.is_open())
                        {
                            for (string line; getline(vertexShaderFile, line);)
                            {
                                if (line.find("uniform") != string::npos)
                                {
                                    bool foundFirstSpace = false;
                                    bool foundSecondSpace = false;

                                    string type;
                                    string name;
                                    for (int i = 0; i < line.size(); i++)
                                    {
                                        if (!foundFirstSpace && line[i] == ' ')
                                        {
                                            foundFirstSpace = true;
                                        }
                                        else if (foundFirstSpace && !foundSecondSpace && line[i] == ' ')
                                        {
                                            foundSecondSpace = true;
                                        }

                                        if (line[i] == ' ' || line[i] == ';')
                                            continue;

                                        if (foundFirstSpace && !foundSecondSpace)
                                        {
                                            type += line[i];
                                        }
                                        else if (foundSecondSpace)
                                        {
                                            name += line[i];
                                        }
                                    }


                                    if (type == "int")
                                    {
                                        int value;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getInt(name.c_str());
                                        sceneFile << "int" << endl;
                                        sceneFile << name << endl;
                                        sceneFile << value << endl;
                                    }
                                    else if (type == "float")
                                    {
                                        float value;
                                        sceneFile << "float" << endl;
                                        sceneFile << name << endl;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getFloat(name.c_str());

                                        sceneFile << value << endl;
                                    }
                                    else if (type == "vec3")
                                    {
                                        glm::vec3 value;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getVec3(name.c_str());
                                        sceneFile << "vec3" << endl;
                                        sceneFile << name << endl;
                                        sceneFile << value.x << ' ' << value.y << ' ' << value.z << endl;
                                    }else if (type == "sampler2D"){
                                        for (int l = 0; l < hashTable[i][j]->NodeModel->meshes[k].textures.size(); l++)
                                        {
                                            if (hashTable[i][j]->NodeModel->meshes[k].textures[l].uniformName == name)
                                            {
                                                sceneFile << "sampler2D" << endl;
                                                sceneFile << name << endl;
                                                sceneFile << "p:" << hashTable[i][j]->NodeModel->meshes[k].textures[l].path << endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        sceneFile << "EOV" << endl; // End of vertex shader uniforms
                        ifstream fragmentShaderFile;
                        fragmentShaderFile.open(hashTable[i][j]->NodeModel->meshes[k].fragmentShaderPath);
                        if (fragmentShaderFile.is_open())
                        {
                            for (string line; getline(fragmentShaderFile, line);)
                            {
                                if (line.find("uniform") != string::npos)
                                {
                                    bool foundFirstSpace = false;
                                    bool foundSecondSpace = false;

                                    string type;
                                    string name;
                                    for (int i = 0; i < line.size(); i++)
                                    {
                                        if (!foundFirstSpace && line[i] == ' ')
                                        {
                                            foundFirstSpace = true;
                                        }
                                        else if (foundFirstSpace && !foundSecondSpace && line[i] == ' ')
                                        {
                                            foundSecondSpace = true;
                                        }

                                        if (line[i] == ' ' || line[i] == ';')
                                            continue;

                                        if (foundFirstSpace && !foundSecondSpace)
                                        {
                                            type += line[i];
                                        }
                                        else if (foundSecondSpace)
                                        {
                                            name += line[i];
                                        }
                                    }

                                    if (type == "int")
                                    {
                                        int value;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getInt(name.c_str());
                                        sceneFile << "int" << endl;
                                        sceneFile << name << endl;
                                        sceneFile << value << endl;
                                    }
                                    else if (type == "float")
                                    {
                                        float value;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getFloat(name.c_str());
                                        sceneFile << "float" << endl;
                                        sceneFile << name << endl;
                                        sceneFile << value << endl;
                                    }
                                    else if (type == "vec3")
                                    {
                                        glm::vec3 value;
                                        value = hashTable[i][j]->NodeModel->meshes[k].shader->getVec3(name.c_str());
                                        sceneFile << "vec3" << endl;
                                        sceneFile << name << endl;
                                        sceneFile << value.x << ' ' << value.y << ' ' << value.z << endl;
                                    }else if (type == "sampler2D"){
                                        for (int l = 0; l < hashTable[i][j]->NodeModel->meshes[k].textures.size(); l++)
                                        {
                                            if (hashTable[i][j]->NodeModel->meshes[k].textures[l].uniformName == name)
                                            {
                                                sceneFile << "sampler2D" << endl;
                                                sceneFile << name << endl;
                                                sceneFile << "p:" << hashTable[i][j]->NodeModel->meshes[k].textures[l].path << endl;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        sceneFile << "EOF" << endl; // End of fragment shader uniforms

                    }
                    sceneFile << "EON" << endl;
                }
                
                
            }
        }

        sceneFile << "EOS" << endl;
       
        sceneFile.close();

        
    }
}

void loadScene()
{
    ifstream sceneFile;
    sceneFile.open("localData/scene.sn");

    if (sceneFile.is_open())
    {
        //if line == "EON" we are done
        string line;
        while (getline(sceneFile, line))
        {
            if (line == "EOS") //emd of scene we read it all
                break;
            else{
                string name;
                name = line;
                unsigned int hashID;
                string path;
                glm::vec3 position;
                glm::vec3 rotation;
                glm::vec3 scale;

                Transform transform;

                vector<unsigned int> childrenHashes;
                sceneFile >> hashID;
                sceneFile >> path;
                sceneFile >> position.x >> position.y >> position.z;
                sceneFile >> rotation.x >> rotation.y >> rotation.z;
                sceneFile >> scale.x >> scale.y >> scale.z;
                
                transform.position = position;
                transform.rotation = rotation;
                transform.scale = scale;

                int numChildren;
                sceneFile >> numChildren;
                bool removeFromDepthBuffer;
                sceneFile >> removeFromDepthBuffer;
                for (int i = 0; i < numChildren; i++)
                {
                    unsigned int childHash;
                    sceneFile >> childHash;
                    childrenHashes.push_back(childHash);
                }

                int numMeshes;
                sceneFile >> numMeshes;
                vector<string> fragmentShaderPaths;
                vector<string> vertexShaderPaths;

                //we need to store the data for the shaders, then after we create the node and model, we can set the shader uniforms based on the data we read in
                //so we need 2d array/vector(s) to store the uniform data for each mesh, and we need to know the type of each uniform so we can set it correctly after we create the model and shader


                for (int i = 0; i < numMeshes; i++)
                {
                    string fragmentShaderPath;
                    sceneFile >> fragmentShaderPath;
                    string vertexShaderPath;
                    sceneFile >> vertexShaderPath;

                    fragmentShaderPaths.push_back(fragmentShaderPath);
                    vertexShaderPaths.push_back(vertexShaderPath);

                    string line;

                    while (getline(sceneFile, line))
                    {
                        if (line == "EOV")
                            break;
                            
                    }
                    while (getline(sceneFile, line))
                    {
                        if (line == "EOF")
                            break;
                          
                    }
                }

                createNewModel(path, vertexShaderPaths[0].c_str(), fragmentShaderPaths[0].c_str(), name, transform, hashID, childrenHashes, removeFromDepthBuffer);

                //loop throught the node and 

                getline(sceneFile, line); //this is EON, aka, end of node
            }
        }

        //we need to link the children object based on the children hashs
        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                SceneTreeNode* node = hashTable[i][j];
                if (node->name == "root"){
                    node->parentNode = sceneRootNode;
                    sceneRootNode->childrenInstances.push_back(node);
                }
                for (int k = 0; k < node->childrenHash.size(); k++)
                {
                    SceneTreeNode* child = getNodeByHash(node->childrenHash[k]);
                    child->parentNode = node;
                    
                    if (child != nullptr)
                    {
                        node->childrenInstances.push_back(child);
                    }
                }
            }
        }
    }
}

void saveData(GLFWwindow* window)
{
    ofstream saveFile;
    saveFile.open("localData/saveData.sv");
    if (saveFile.is_open())
    {
        cout << "save file opened" << endl;

        saveFile << PanSensitivity << endl;
        saveFile << RotateSensitivity << endl;
        saveFile << ForwardSensitivity << endl;

        for (int i = 0; i < 3; i++)
            saveFile << skyColor[i] << ' ';
        saveFile << endl;
        for (int i = 0; i < 3; i++)
            saveFile << dirLightAmbientColor[i] << ' ';
        saveFile << endl;
        for (int i = 0; i < 3; i++)
            saveFile << dirLightDiffuseColor[i] << ' ';
        saveFile << endl;
        for (int i = 0; i < 3; i++)
            saveFile << dirLightSpecularColor[i] << ' ';
        saveFile << endl;

        if (SCR_HEIGHT == 1)
            SCR_HEIGHT = 720;
        if (SCR_WIDTH == 1)
            SCR_WIDTH = 1280;
        saveFile << SCR_HEIGHT << endl;
        saveFile << SCR_WIDTH << endl;

        saveFile << currentPath << endl;
        saveFile << cameraFOV << endl;

        for (int i = 0; i < 3; i++)
            saveFile << dirLightDirection[i] << ' ';

        for (int i = 0; i < 3; i++)
            saveFile << camera.Position[i] << ' ';

        for (int i = 0; i < 4; i++)
            saveFile << camera.Orientation[i] << ' ';
        for (int i = 0; i < 3; i++)
            saveFile << camera.Front[i] << ' ';
        for (int i = 0; i < 3; i++)
            saveFile << camera.Up[i] << ' ';

        saveFile << depthLinearizationNear << endl;
        saveFile << depthLinearizationFar << endl;
        saveFile << fogColor[0] << ' ' << fogColor[1] << ' ' << fogColor[2] << ' ' << endl;

        saveFile << SCR_HEIGHT << endl;
        saveFile << SCR_WIDTH << endl;
        int windowPosX, windowPosY;
        glfwGetWindowPos(window, &windowPosX, &windowPosY);
        saveFile << windowPosX << ' ' << windowPosY << endl;


        saveFile.close();
    }
}

void loadData(GLFWwindow* window)
{
    ifstream saveFile;
    saveFile.open("localData/saveData.sv");
    if (saveFile.is_open())
    {
        cout << "save file opened" << endl;

        saveFile >> PanSensitivity;
        saveFile >> RotateSensitivity;
        saveFile >> ForwardSensitivity;

        for (int i = 0; i < 3; i++)
            saveFile >> skyColor[i];
        for (int i = 0; i < 3; i++)
            saveFile >> dirLightAmbientColor[i];
        for (int i = 0; i < 3; i++)
            saveFile >> dirLightDiffuseColor[i];
        for (int i = 0; i < 3; i++)
            saveFile >> dirLightSpecularColor[i];

        saveFile >> SCR_HEIGHT;
        saveFile >> SCR_WIDTH;

        saveFile >> currentPath;
        saveFile >> cameraFOV;

        for (int i = 0; i < 3; i++)
            saveFile >> dirLightDirection[i];

        for (int i = 0; i < 3; i++)
            saveFile >> camera.Position[i];
        for (int i = 0; i < 4; i++)
            saveFile >> camera.Orientation[i];
        for (int i = 0; i < 3; i++)
            saveFile >> camera.Front[i];
        for (int i = 0; i < 3; i++)
            saveFile >> camera.Up[i];

        saveFile >> depthLinearizationNear;
        saveFile >> depthLinearizationFar;
        saveFile >> fogColor[0] >> fogColor[1] >> fogColor[2];

        screenShader->use();
        screenShader->setVec3("fogColor", glm::vec3(fogColor[0], fogColor[1], fogColor[2]));
        screenShader->setFloat("near", depthLinearizationNear);
        screenShader->setFloat("far", depthLinearizationFar);

        saveFile >> SCR_HEIGHT;
        saveFile >> SCR_WIDTH;
        int windowPosX, windowPosY;
        saveFile >> windowPosX >> windowPosY;
        glfwSetWindowPos(window, windowPosX, windowPosY);
        glfwSetWindowSize(window, SCR_WIDTH, SCR_HEIGHT);

        saveFile.close();
    }
}

void drawShaderSelection(int *selectedShader, string shaderType)
{
    ImGui::PushID(shaderType.c_str());
    if (ImGui::Button(("Select " + shaderType).c_str()))
        ImGui::OpenPopup((shaderType + "_select_popup").c_str());
    ImGui::SameLine();
    ImGui::Text(shaderType.c_str());
    if (ImGui::BeginPopup((shaderType + "_select_popup").c_str()))
    {
        ImGui::SeparatorText(("Select " + shaderType).c_str());
        for (int i = 0; i < shaderNames.size(); i++)
            if (ImGui::Selectable(shaderNames[i].c_str()))
            {
                *selectedShader = i; // this is where we set the shader to the selected one
                if (shaderType == "Vertex Shader")
                    selectedNode->NodeModel->meshes[selected].vertexShaderPath = (path + '/' + shaderNames[i]).c_str();
                else if (shaderType == "Fragment Shader")
                {
                    selectedNode->NodeModel->meshes[selected].fragmentShaderPath = (path + '/' + shaderNames[i]).c_str();
                }
                selectedNode->NodeModel->meshes[selected].reloadShaders();
            }
        ImGui::EndPopup();
    }
    ImGui::PopID();
}

void resetData()
{
    skyColor[0] = 0.4f;
    skyColor[1] = 0.4f;
    skyColor[2] = 0.9f;
    dirLightDiffuseColor[0] = 0.5f;
    dirLightDiffuseColor[1] = 0.5f;
    dirLightDiffuseColor[2] = 0.5f;
    dirLightAmbientColor[0] = 0.2f;
    dirLightAmbientColor[1] = 0.2f;
    dirLightAmbientColor[2] = 0.2f;
    dirLightSpecularColor[0] = 1.0f;
    dirLightSpecularColor[1] = 1.0f;
    dirLightSpecularColor[2] = 1.0f;

    dirLightDirection[0] = -0.2f;
    dirLightDirection[1] = -1.0f;
    dirLightDirection[2] = -0.3f;

    PanSensitivity = 1.0f;
    RotateSensitivity = 1.0f;
    ForwardSensitivity = 1.0f;
    cameraFOV = 45.0f;

    depthLinearizationNear = 0.1f;
    depthLinearizationFar = 100.0f;
    fogColor[0] = 0.5f;
    fogColor[1] = 0.5f;
    fogColor[2] = 0.5f;

    currentPath = fs::current_path();
}

void drawMainUI(GLFWwindow* window)
{
    ImGui::Begin("OpenGL UI");
    ImGui::Text("FPS: %.1f", deltaTime != 0.0f ? (1.0f / deltaTime) : 0.0f);

    static float values[90] = {};
    static int values_offset = 0;
    static double refresh_time = 0.0;

    while (refresh_time < ImGui::GetTime()) // Create data at fixed 60 Hz rate for the demo
    {
        static float phase = 0.0f;
        values[values_offset] = cosf(phase);
        values_offset = (values_offset + 1) % 90;
        phase += 0.10f * values_offset;
        refresh_time += 1.0f / 60.0f;
    }

    // Plots can display overlay texts
    // (in this example, we will display an average value)
    {
        float average = 0.0f;
        for (int n = 0; n < 90; n++)
            average += values[n];
        average /= 90.0f;
        char overlay[32];
        sprintf(overlay, "avg %f", average);
        ImGui::PlotLines("frametime", values, 90, values_offset, overlay, -1.0f, 1.0f, ImVec2(0, 60.0f));
    }

    ImGui::Text("Number of Vertices: %d", numberOfVertices);
    ImGui::Text("Number of Batches: %d", numberOfBatches);

    ImGui::SliderFloat("RotateSensitivity", &RotateSensitivity, 0.1f, 5.0f);
    ImGui::SliderFloat("PanSensitivity", &PanSensitivity, 0.1f, 5.0f);
    ImGui::SliderFloat("ForwardSensitivity", &ForwardSensitivity, 0.1f, 5.0f);
    ImGui::SliderFloat("CameraFOV", &cameraFOV, 45.0f, 120.0f);

    ImGui::ColorEdit3("SkyColor", skyColor);
    ImGui::DragFloat3("DirLightDirection", dirLightDirection, 0.1f);

    ImGui::ColorEdit3("DirLightDiffuseColor", dirLightDiffuseColor);
    ImGui::ColorEdit3("DirLightAmbientColor", dirLightAmbientColor);
    ImGui::ColorEdit3("DirLightSpecularColor", dirLightSpecularColor);

    
    ImGui::DragFloat("Depth Linearization Near", &depthLinearizationNear, 0.00001f, 0.0f, 0.0f, "%.06f");
    ImGui::SliderFloat("Depth Linearization Far", &depthLinearizationFar, 1.0f, 20000.0f);
    ImGui::ColorEdit3("Fog Color", fogColor);
    if (ImGui::Button("Apply Depth Linearization Values"))
    {
        screenShader->use();

        screenShader->setVec3("fogColor", glm::vec3(fogColor[0], fogColor[1], fogColor[2]));
        screenShader->setFloat("near", depthLinearizationNear);
        screenShader->setFloat("far", depthLinearizationFar);
    }

    if (ImGui::Button("Save Data"))
    {
        saveData(window);
    }
    if (ImGui::Button("reset save data"))
    {
        resetData();
    }

    if (ImGui::Button("Save Scene"))
    {
        saveScene();
    }
    if (ImGui::Button("Load Scene"))
    {
        loadScene();
    }
    if (ImGui::Button("reload shaders"))
    {

        delete screenShader;
        screenShader = new Shader("resources/shaders/framebuffers_screen_vert.glsl", "resources/shaders/framebuffers_screen_frag.glsl");
        screenShader->use();
        screenShader->setInt("screenTexture", 0);
        screenShader->setInt("depthTexture", 1);


        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                if (hashTable[i][j] == sceneRootNode || hashTable[i][j]->NodeModel == nullptr)
                    continue;
                for (int selected = 0; selected < hashTable[i][j]->NodeModel->meshes.size(); selected++)
                {
                    hashTable[i][j]->NodeModel->meshes[selected].reloadShaders();
                }
            }
        }
    }
    ImGui::End();
}

void ShowFileBrowser()
{
    ImGui::Begin("File Browser");

    // Back button
    if (ImGui::Button("⬅ Up") && currentPath.has_parent_path())
    {
        currentPath = currentPath.parent_path();
    }

    ImGui::Separator();
    ImGui::Text("Current Path: %s", currentPath.string().c_str());
    ImGui::Separator();
    std::string filename;

    // Iterate through directory
    for (auto &entry : fs::directory_iterator(currentPath))
    {
        const auto &path = entry.path();
        filename = path.filename().string();

        if (entry.is_directory())
        {
            // Folders: click to enter
            if (ImGui::Selectable((filename + "/").c_str(), false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    currentPath = path;
                }
            }
        }
        else
        {
            // Files: click to select
            if (ImGui::Selectable(filename.c_str(), selectedFile == filename))
            {
                selectedFile = filename;
                cout << "Selected file path: " << currentPath << '\\' << filename << endl;
            }
        }
    }

    if (ImGui::Button("Spawn Model?"))
    {
        if (selectedNode == nullptr)
        {
            ImGui::End();
            return;
        }
        if (selectedNode != sceneRootNode)
        {
            Transform transform{camera.Position + camera.Front * 0.3f, glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};
            createNewModel((currentPath / selectedFile).string().c_str(), "resources/shaders/litObject_vertex.glsl", "resources/shaders/litObject_fragment.glsl", selectedFile, transform, selectedNode);
        }
    }

    
    ImGui::End();
}

float doubleClickTime = 0.90f;
bool clicked = false;

void drawSceneTreeHierarchical(SceneTreeNode *node, SceneTreeNode *&selectedNode)
{

    // ok so first start from the root node
    // if the dropdown is open, then we need to draw the children
    // if any of those children are selected, we need to draw them as well

    // label for this node
    std::string label = node->name;

    // is this node selected?
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               (node->childrenInstances.empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
                               ((selectedNode == node) ? ImGuiTreeNodeFlags_Selected : 0);

    
    bool open = ImGui::TreeNodeEx((void *)node, flags, "%s", label.c_str());

    // handle selection
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
    {
        selectedNode = node;

        if (clicked && doubleClickTime > 0)
        {
            // double clicked a node

            glm::vec3 position = selectedNode->transform.position;
            glm::vec3 position2 = camera.Position + camera.Front * 2.0f;

            glm::vec3 difference = position2 - position;

            camera.Position -= difference;
        }

        clicked = true;
    }

    if (clicked)
    {
        doubleClickTime -= deltaTime;
    }

    if (doubleClickTime <= 0)
    {
        doubleClickTime = 0.90f;
        clicked = false;
    }

    // draw children recursively
    if (open)
    {
        for (SceneTreeNode *child : node->childrenInstances)
            drawSceneTreeHierarchical(child, selectedNode);

        ImGui::TreePop();
    }
}

void drawSceneTree()
{
    ImGui::Begin("Scene Tree");

    drawSceneTreeHierarchical(sceneRootNode, selectedNode);

    if (selectedNode == sceneRootNode)
    {
        selectedNode = nullptr;
    }

    if (selectedNode)
    {
        ImGui::Separator();
        std::snprintf(selectedName, sizeof(selectedName), "%s", selectedNode->name.c_str());
        if (ImGui::InputText("Name", selectedName, IM_ARRAYSIZE(selectedName)))
        {
            selectedNode->name = selectedName;
        }
        ImGui::DragFloat3("Position", glm::value_ptr(selectedNode->transform.position), 0.001f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(selectedNode->transform.rotation), 1.0f);
        ImGui::DragFloat3("Scale", glm::value_ptr(selectedNode->transform.scale), 0.1f, 0.1f, 10.0f);



        if (ImGui::Button("Reset Transform"))
        {
            Transform transform{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};
            selectedNode->transform = transform;
        }
        ImGui::Checkbox("Remove from Depth Buffer", &selectedNode->NodeModel->removeFromDepthBuffer);
        
        ImGui::Text("Hash ID: %zu", selectedNode->hashID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;

        if (ImGui::CollapsingHeader("Mesh Selection:", ImGuiTreeNodeFlags_None))
        {
            int verticalSize = 0;
            if (selectedNode->NodeModel->meshes.size() > 7)
                verticalSize = 160;
            else
                verticalSize = selectedNode->NodeModel->meshes.size() * 15;

            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.95, verticalSize), ImGuiChildFlags_None, window_flags);
            for (int i = 0; i < selectedNode->NodeModel->meshes.size(); i++)
            {
                char buf[32];
                sprintf(buf, "Mesh %d", i);
                if (ImGui::Selectable(buf, selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }

        if (ImGui::CollapsingHeader("Set Shader properties:", ImGuiTreeNodeFlags_None) && selected != -1)
        {

            ImGui::BeginChild("shaderChild", ImVec2(ImGui::GetContentRegionAvail().x * 0.95, 300), ImGuiChildFlags_None, window_flags);
            char vertexShaderBuffer[512];
            char fragmentShaderBuffer[512];

            vertexShaderName = selectedNode->NodeModel->meshes[selected].vertexShaderPath.substr(selectedNode->NodeModel->meshes[selected].vertexShaderPath.find_last_of("/\\") + 1);
            fragmentShaderName = selectedNode->NodeModel->meshes[selected].fragmentShaderPath.substr(selectedNode->NodeModel->meshes[selected].fragmentShaderPath.find_last_of("/\\") + 1);

            std::snprintf(vertexShaderBuffer, sizeof(vertexShaderBuffer), "%s", vertexShaderName.c_str());
            std::snprintf(fragmentShaderBuffer, sizeof(fragmentShaderBuffer), "%s", fragmentShaderName.c_str());

            static int selected_vertex = -1;
            static int selected_fragment = -1;
            drawShaderSelection(&selected_vertex, "Vertex Shader");
            drawShaderSelection(&selected_fragment, "Fragment Shader");

            if (selected != previousSelected || selectedNode != previousSelectedNode)
            {
                alreadyCreated = false;
                vertexUniforms.clear();
                vertexUniformTypes.clear();
                vertexUniformInts.clear();
                vertexUniformFloats.clear();
                vertexUniformVec3s.clear();

                fragmentUniforms.clear();
                fragmentUniformTypes.clear();
                fragmentUniformInts.clear();
                fragmentUniformFloats.clear();
                fragmentUniformVec3s.clear();

                shaderNames.clear();


                selectedVertexTextureIDs.clear();
                selectedFragmentTextureIDs.clear();

                textureNames.clear();
                texturePaths.clear();

                selected_vertex = -1;
                selected_fragment = -1;

                struct stat sb;
                for (const auto &entry : fs::directory_iterator(path))
                {

                    // Converting the path to const char * in the
                    // subsequent lines
                    std::filesystem::path outfilename = entry.path();
                    std::string outfilename_str = outfilename.string();
                    const char *path = outfilename_str.c_str();

                    // Testing whether the path points to a
                    // non-directory or not If it does, displays path
                    if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR))
                        shaderNames.push_back(outfilename.filename().string());

                }


                for (const auto &entry : fs::directory_iterator(texturePath))
                {

                    // Converting the path to const char * in the
                    // subsequent lines
                    std::filesystem::path outfilename = entry.path();
                    std::string outfilename_str = outfilename.string();
                    const char *path = outfilename_str.c_str();

                    // Testing whether the path points to a
                    // non-directory or not If it does, displays path
                    if (stat(path, &sb) == 0 && !(sb.st_mode & S_IFDIR)){
                        textureNames.push_back(outfilename.filename().string());
                        texturePaths.push_back(outfilename_str);
                        selectedFragmentTextureIDs.push_back(-1);
                        selectedVertexTextureIDs.push_back(-1);
                    }
                }

                previousSelected = selected;
                previousSelectedNode = selectedNode;
            }

            if (!alreadyCreated)
            {
                // lets try to parse the shaders so we can set values easier
                ifstream vertexShaderFile;
                ifstream fragmentShaderFile;
                vertexShaderFile.open(selectedNode->NodeModel->meshes[selected].vertexShaderPath);
                fragmentShaderFile.open(selectedNode->NodeModel->meshes[selected].fragmentShaderPath);

                for (string line; getline(vertexShaderFile, line);)
                {
                    if (line.find("uniform") != string::npos)
                    {
                        // get data type and name
                        bool foundFirstSpace = false;
                        bool foundSecondSpace = false;

                        string type;
                        string name;
                        for (int i = 0; i < line.size(); i++)
                        {
                            if (!foundFirstSpace && line[i] == ' ')
                            {
                                foundFirstSpace = true;
                                // while this is false we found "uniform" so we can start looking for the type
                            }
                            else if (foundFirstSpace && !foundSecondSpace && line[i] == ' ')
                            {
                                foundSecondSpace = true;
                                // this is the type of the uniform
                            }

                            if (line[i] == ' ' || line[i] == ';')
                                continue;

                            if (foundFirstSpace && !foundSecondSpace)
                            {
                                type += line[i];
                            }
                            else if (foundSecondSpace)
                            {
                                name += line[i];
                            }
                        }

                        vertexUniforms.push_back(name);
                        vertexUniformTypes.push_back(type);
                    }
                }

                fragmentUniforms.clear();
                fragmentUniformTypes.clear();
                for (string line; getline(fragmentShaderFile, line);)
                {
                    if (line.find("uniform") != string::npos)
                    {
                        // get data type and name
                        bool foundFirstSpace = false;
                        bool foundSecondSpace = false;

                        string type;
                        string name;
                        for (int i = 0; i < line.size(); i++)
                        {
                            if (!foundFirstSpace && line[i] == ' ')
                            {
                                foundFirstSpace = true;
                                // while this is false we found "uniform" so we can start looking for the type
                            }
                            else if (foundFirstSpace && !foundSecondSpace && line[i] == ' ')
                            {
                                foundSecondSpace = true;
                                // this is the type of the uniform
                            }

                            if (line[i] == ' ' || line[i] == ';')
                                continue;

                            if (foundFirstSpace && !foundSecondSpace)
                            {
                                type += line[i];
                            }
                            else if (foundSecondSpace)
                            {
                                name += line[i];
                            }
                        }

                        fragmentUniforms.push_back(name);
                        fragmentUniformTypes.push_back(type);
                    }
                }

                vertexShaderFile.close();
                fragmentShaderFile.close();
            }

            ImGui::Separator();
            floatCounter = 0;
            vec3Counter = 0;
            intCounter = 0;
            textureIDCounter = 0;

            ImGui::Text("Vertex Shader Uniforms:");
            for (int i = 0; i < vertexUniforms.size(); i++)
            {
                ImGui::Text("%s: ", vertexUniforms[i].c_str());
                if (vertexUniformTypes[i] == "float")
                {

                    if (!alreadyCreated)
                        vertexUniformFloats.push_back(selectedNode->NodeModel->meshes[selected].shader->getFloat(vertexUniforms[i].c_str()));
                    if (ImGui::SliderFloat(vertexUniforms[i].c_str(), &vertexUniformFloats[floatCounter], 0.0f, 5.0f))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setFloat(vertexUniforms[i].c_str(), vertexUniformFloats[floatCounter]);
                    }
                    floatCounter++;
                }
                else if (vertexUniformTypes[i] == "vec3")
                {
                    if (!alreadyCreated)
                        vertexUniformVec3s.push_back(selectedNode->NodeModel->meshes[selected].shader->getVec3(vertexUniforms[i].c_str()));
                    if (ImGui::SliderFloat3(vertexUniforms[i].c_str(), &vertexUniformVec3s[vec3Counter][0], 0.0f, 5.0f))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setVec3(vertexUniforms[i].c_str(), vertexUniformVec3s[vec3Counter][0], vertexUniformVec3s[vec3Counter][1], vertexUniformVec3s[vec3Counter][2]);
                    }
                    vec3Counter++;
                }
                else if (vertexUniformTypes[i] == "int")
                {
                    if (!alreadyCreated)
                        vertexUniformInts.push_back(selectedNode->NodeModel->meshes[selected].shader->getInt(vertexUniforms[i].c_str()));
                    if (ImGui::SliderInt(vertexUniforms[i].c_str(), &vertexUniformInts[intCounter], 0, 5))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setInt(vertexUniforms[i].c_str(), vertexUniformInts[intCounter]);
                    }
                    intCounter++;
                }
                else if (vertexUniformTypes[i] == "sampler2D")
                {

                    Texture *currentTexture = nullptr;
                    for (int j = 0; j < selectedNode->NodeModel->meshes[selected].textures.size(); j++)
                    {
                        if (selectedNode->NodeModel->meshes[selected].textures[j].uniformName == fragmentUniforms[i])
                        {
                            currentTexture = &selectedNode->NodeModel->meshes[selected].textures[j];
                            break;
                        }
                    }

                    if (currentTexture != nullptr)
                    {

                        ImTextureID imguiTex = (ImTextureID)(intptr_t)currentTexture->id;
                        ImVec2 uv_min(0.0f, 0.0f);
                        ImVec2 uv_max(1.0f, 1.0f);
                        ImGui::PushStyleVar(
                            ImGuiStyleVar_ImageBorderSize,
                            max(1.0f, ImGui::GetStyle().ImageBorderSize));

                        ImGui::ImageWithBg(
                            imguiTex,
                            ImVec2(128, 128),
                            uv_min,
                            uv_max,
                            ImVec4(0, 0, 0, 1));

                        ImGui::PopStyleVar();

                        // ImGui::SameLine();
                    }
                    // Simple selection popup (if you want to show the current selection inside the Button itself,
                    // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
                    // static int selected_fragment = -1;
                    ImGui::PushID(i);

                    if (ImGui::Button(("Select Texture" + std::to_string(i)).c_str()))
                        ImGui::OpenPopup(("texture_select_popup" + std::to_string(i)).c_str());
                    ImGui::SameLine();
                    // ImGui::Text("Fragment Shader");
                    if (ImGui::BeginPopup(("texture_select_popup" + std::to_string(i)).c_str()))
                    {

                        ImGui::SeparatorText("textures");
                        if (ImGui::Selectable("None"))
                        {
                            // Handle "None" selection
                            // just remove from the textures array
                            selectedNode->NodeModel->meshes[selected].textures.erase(std::remove_if(selectedNode->NodeModel->meshes[selected].textures.begin(), selectedNode->NodeModel->meshes[selected].textures.end(),
                                                                                                    [&](const Texture &t)
                                                                                                    { return t.uniformName == fragmentUniforms[i]; }),
                                                                                     selectedNode->NodeModel->meshes[selected].textures.end());
                        }
                        for (int k = 0; k < textureNames.size(); k++)
                            if (ImGui::Selectable(textureNames[k].c_str()))
                            {
                                selectedFragmentTextureIDs[k] = k; // this is where we set the shader to the selected one
                                cout << "Selected texture: " << textureNames[k] << endl;
                                // ight so what we need to do to load a texture is this,
                                // we need to load the texture
                                Texture texture;
                                unsigned int id = selectedNode->NodeModel->TextureFromFile(texturePaths[k].c_str(), selectedNode->NodeModel->directory, false);
                                // when we do that we need to set the parameters correctly with the uniform name too
                                if (id != 4294967295)
                                {
                                    texture.id = id;
                                    texture.type = fragmentUniforms[i];
                                    texture.path = texturePaths[k].c_str();

                                    if (currentTexture == nullptr)
                                    {
                                        selectedNode->NodeModel->meshes[selected].textures.push_back(texture);
                                    }
                                    else
                                    {
                                        // remove old texture and add the new one
                                        selectedNode->NodeModel->meshes[selected].textures.erase(std::remove_if(selectedNode->NodeModel->meshes[selected].textures.begin(), selectedNode->NodeModel->meshes[selected].textures.end(),
                                                                                                                [&](const Texture &t)
                                                                                                                { return t.uniformName == fragmentUniforms[i]; }),
                                                                                                 selectedNode->NodeModel->meshes[selected].textures.end());
                                    }
                                    selectedNode->NodeModel->meshes[selected].textures.push_back(texture);

                                    selectedNode->NodeModel->meshes[selected].shader->use();
                                    selectedNode->NodeModel->meshes[selected].shader->setInt(fragmentUniforms[i].c_str(), selectedNode->NodeModel->meshes[selected].textures.size() - 1);
                                    alreadyCreated = false;
                                }
                                // then i think bind and set the int(sampler2D)
                            }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                    textureIDCounter++;
                }
                else
                {
                    ImGui::Text("Unknown uniform type: %s", vertexUniformTypes[i].c_str());
                }
            }

            intCounter = 0;
            floatCounter = 0;
            vec3Counter = 0;
            textureIDCounter = 0;

            ImGui::Text("Fragment Shader Uniforms:");
            for (int i = 0; i < fragmentUniforms.size(); i++)
            {
                ImGui::Text("%s: ", fragmentUniforms[i].c_str());
                if (fragmentUniformTypes[i] == "float")
                {
                    if (!alreadyCreated)
                        fragmentUniformFloats.push_back(selectedNode->NodeModel->meshes[selected].shader->getFloat(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderFloat(fragmentUniforms[i].c_str(), &fragmentUniformFloats[floatCounter], 0.0f, 5.0f))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setFloat(fragmentUniforms[i].c_str(), fragmentUniformFloats[floatCounter]);
                    }
                    floatCounter++;
                }
                else if (fragmentUniformTypes[i] == "vec3")
                {
                    if (!alreadyCreated)
                        fragmentUniformVec3s.push_back(selectedNode->NodeModel->meshes[selected].shader->getVec3(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderFloat3(fragmentUniforms[i].c_str(), &fragmentUniformVec3s[vec3Counter][0], 0.0f, 5.0f))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setVec3(fragmentUniforms[i].c_str(), fragmentUniformVec3s[vec3Counter][0], fragmentUniformVec3s[vec3Counter][1], fragmentUniformVec3s[vec3Counter][2]);
                    }
                    vec3Counter++;
                }
                else if (fragmentUniformTypes[i] == "int")
                {
                    if (!alreadyCreated)
                        fragmentUniformInts.push_back(selectedNode->NodeModel->meshes[selected].shader->getInt(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderInt(fragmentUniforms[i].c_str(), &fragmentUniformInts[intCounter], 0, 5))
                    {
                        selectedNode->NodeModel->meshes[selected].shader->use();
                        selectedNode->NodeModel->meshes[selected].shader->setInt(fragmentUniforms[i].c_str(), fragmentUniformInts[intCounter]);
                    }
                    intCounter++;
                }
                else if (fragmentUniformTypes[i] == "sampler2D")
                {
                    //for each sampler2D we need the image loaded if it has one
                    //and the selection thing for all of them

                    Texture *currentTexture = nullptr;
                    for (int j = 0; j < selectedNode->NodeModel->meshes[selected].textures.size(); j++)
                    {
                        if (selectedNode->NodeModel->meshes[selected].textures[j].uniformName == fragmentUniforms[i])
                        {
                            currentTexture = &selectedNode->NodeModel->meshes[selected].textures[j];
                            break;
                        }
                    }

                    if (currentTexture != nullptr)
                    {

                        ImTextureID imguiTex = (ImTextureID)(intptr_t)currentTexture->id;
                        ImVec2 uv_min(0.0f, 0.0f);
                        ImVec2 uv_max(1.0f, 1.0f);
                        ImGui::PushStyleVar(
                            ImGuiStyleVar_ImageBorderSize,
                            max(1.0f, ImGui::GetStyle().ImageBorderSize));

                        ImGui::ImageWithBg(
                            imguiTex,
                            ImVec2(128, 128),
                            uv_min,
                            uv_max,
                            ImVec4(0, 0, 0, 1));

                        ImGui::PopStyleVar();

                        // ImGui::SameLine();
                    }
                    // Simple selection popup (if you want to show the current selection inside the Button itself,
                    // you may want to build a string using the "###" operator to preserve a constant ID with a variable label)
                    // static int selected_fragment = -1;
                    ImGui::PushID(i);

                    if (ImGui::Button(("Select Texture" + std::to_string(i)).c_str()))
                        ImGui::OpenPopup(("texture_select_popup" + std::to_string(i)).c_str());
                    ImGui::SameLine();
                    // ImGui::Text("Fragment Shader");
                    if (ImGui::BeginPopup(("texture_select_popup" + std::to_string(i)).c_str()))
                    {

                        ImGui::SeparatorText("textures");
                        if (ImGui::Selectable("None"))
                        {
                            // Handle "None" selection
                            // just remove from the textures array
                            selectedNode->NodeModel->meshes[selected].textures.erase(std::remove_if(selectedNode->NodeModel->meshes[selected].textures.begin(), selectedNode->NodeModel->meshes[selected].textures.end(),
                                                                                                    [&](const Texture &t)
                                                                                                    { return t.uniformName == fragmentUniforms[i]; }),
                                                                                     selectedNode->NodeModel->meshes[selected].textures.end());
                        }
                        for (int k = 0; k < textureNames.size(); k++)
                            if (ImGui::Selectable(textureNames[k].c_str()))
                            {
                                selectedFragmentTextureIDs[k] = k; // this is where we set the shader to the selected one
                                cout << "Selected texture: " << textureNames[k] << endl;
                                // ight so what we need to do to load a texture is this,
                                // we need to load the texture
                                Texture texture;
                                unsigned int id = selectedNode->NodeModel->TextureFromFile(texturePaths[k].c_str(), selectedNode->NodeModel->directory, false);
                                // when we do that we need to set the parameters correctly with the uniform name too
                                if (id != 4294967295)
                                {
                                    texture.id = id;
                                    texture.type = fragmentUniforms[i];
                                    texture.path = texturePaths[k].c_str();

                                    if (currentTexture == nullptr){
                                        selectedNode->NodeModel->meshes[selected].textures.push_back(texture);
                                    }else{
                                        //remove old texture and add the new one
                                        selectedNode->NodeModel->meshes[selected].textures.erase(std::remove_if(selectedNode->NodeModel->meshes[selected].textures.begin(), selectedNode->NodeModel->meshes[selected].textures.end(),
                                        [&](const Texture& t) { return t.uniformName == fragmentUniforms[i]; }), selectedNode->NodeModel->meshes[selected].textures.end());
                                    }
                                    selectedNode->NodeModel->meshes[selected].textures.push_back(texture);

                                    selectedNode->NodeModel->meshes[selected].shader->use();
                                    selectedNode->NodeModel->meshes[selected].shader->setInt(fragmentUniforms[i].c_str(), selectedNode->NodeModel->meshes[selected].textures.size() - 1);
                                    alreadyCreated = false;
                                }
                                // then i think bind and set the int(sampler2D)
                            }
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                    textureIDCounter++;
                }
            }

            if (!alreadyCreated)
            {
                alreadyCreated = true;
            }
            ImGui::EndChild();
        }
        ImGui::Separator();

        if (ImGui::Button("copy shader properties to all meshes"))
        {
            string fragPath = selectedNode->NodeModel->meshes[selected].fragmentShaderPath;
            string vertPath = selectedNode->NodeModel->meshes[selected].vertexShaderPath;

            for (int i = 0; i < selectedNode->NodeModel->meshes.size(); i++)
            {
                selectedNode->NodeModel->meshes[i].vertexShaderPath = vertPath;
                selectedNode->NodeModel->meshes[i].fragmentShaderPath = fragPath;
                selectedNode->NodeModel->meshes[i].reloadShaders();

                int intCounter = 0;
                int floatCounter = 0;
                int vec3Counter = 0;

                for (int j = 0; j < vertexUniforms.size(); j++)
                {
                    selectedNode->NodeModel->meshes[i].shader->use();
                    if (vertexUniformTypes[j] == "float")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setFloat(vertexUniforms[j].c_str(), vertexUniformFloats[floatCounter]);
                        floatCounter++;
                    }
                    else if (vertexUniformTypes[j] == "vec3")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setVec3(vertexUniforms[j].c_str(), vertexUniformVec3s[vec3Counter][0], vertexUniformVec3s[vec3Counter][1], vertexUniformVec3s[vec3Counter][2]);
                        vec3Counter++;
                    }
                    else if (vertexUniformTypes[j] == "int")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setInt(vertexUniforms[j].c_str(), vertexUniformInts[intCounter]);
                        intCounter++;
                    }
                }

                intCounter = 0;
                floatCounter = 0;
                vec3Counter = 0;

                for (int j = 0; j < fragmentUniforms.size(); j++)
                {
                    
                    selectedNode->NodeModel->meshes[i].shader->use();
                    if (fragmentUniformTypes[j] == "float")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setFloat(fragmentUniforms[j].c_str(), fragmentUniformFloats[floatCounter]);
                        floatCounter++;
                    }
                    else if (fragmentUniformTypes[j] == "vec3")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setVec3(fragmentUniforms[j].c_str(), fragmentUniformVec3s[vec3Counter][0], fragmentUniformVec3s[vec3Counter][1], fragmentUniformVec3s[vec3Counter][2]);
                        vec3Counter++;
                    }
                    else if (fragmentUniformTypes[j] == "int")
                    {
                        selectedNode->NodeModel->meshes[i].shader->setInt(fragmentUniforms[j].c_str(), fragmentUniformInts[intCounter]);
                        intCounter++;
                    }
                }
            }
        }

        if (ImGui::Button("Reload Selected Shader"))
        {
            for (int i = 0; i < selectedNode->NodeModel->meshes.size(); i++)
            {
                selectedNode->NodeModel->meshes[i].reloadShaders();
            }
        }

        if (ImGui::Button("Delete"))
        {
            SceneTreeNode *nodeToDelete = selectedNode;
            SceneTreeNode *parentNode = nodeToDelete->parentNode;

            removeNodeFromSceneTree(nodeToDelete);

            selectedNode = nullptr;
        }
    }

    ImGui::End();
}

void drawAllUI(GLFWwindow* window)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainUI(window);

    drawSceneTree();
    ShowFileBrowser();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif