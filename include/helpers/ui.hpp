#ifndef ui_hpp
#define ui_hpp

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

//for shader editing
bool alreadyCreated = false;
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

char selectedName[256] = "";

void saveScene()
{
    // save the scene to a file
    ofstream sceneFile;
    sceneFile.open("localData/scene.sn");
    if (sceneFile.is_open())
    {

        vector<string> uniformNames;
        vector<string> uniformTypes;
        vector<int> uniformInts;
        vector<float> uniformFloats;
        vector<glm::vec3> uniformVec3s;

        /*
        sceneFile << sceneModels.size() << endl;
        for (int i = 1; i < sceneModels.size(); i++)
        {
            SceneTreeNode *node = sceneModels[i];
            Model *model = node->NodeModel;
            sceneFile << model->directory << endl;
            sceneFile << model->instanceCount << endl;
            sceneFile << model->vertexShaderPath << endl;
            sceneFile << model->fragmentShaderPath << endl;

            //we need to save the shader properties
            //so parse the shader files, get the properties name, type and value. then save them
            ifstream vertexShaderFile;
            vertexShaderFile.open(model->vertexShaderPath);
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

                        sceneFile << name << endl;
                        sceneFile << type << endl;

                        if (type == "int")
                        {
                            int value;
                            value = model->shader->getInt(name.c_str());
                            uniformInts.push_back(value);
                            sceneFile << value << endl;
                        }
                        else if (type == "float")
                        {
                            float value;
                            value = model->shader->getFloat(name.c_str());
                            uniformFloats.push_back(value);
                            sceneFile << value << endl;
                        }
                        else if (type == "vec3")
                        {
                            glm::vec3 value;
                            value = model->shader->getVec3(name.c_str());
                            uniformVec3s.push_back(value);
                            sceneFile << value.x << ' ' << value.y << ' ' << value.z << endl;
                        }
                    }
                }
            }
            sceneFile << "EOV" << endl; // End of vertex shader uniforms
            ifstream fragmentShaderFile;
            fragmentShaderFile.open(model->fragmentShaderPath);
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

                        sceneFile << name << endl;
                        sceneFile << type << endl;

                        if (type == "int")
                        {
                            int value;
                            value = model->shader->getInt(name.c_str());
                            uniformInts.push_back(value);
                            sceneFile << value << endl;
                        }
                        else if (type == "float")
                        {
                            float value;
                            value = model->shader->getFloat(name.c_str());
                            uniformFloats.push_back(value);
                            sceneFile << value << endl;
                        }
                        else if (type == "vec3")
                        {
                            glm::vec3 value;
                            value = model->shader->getVec3(name.c_str());
                            uniformVec3s.push_back(value);
                            sceneFile << value.x << ' ' << value.y << ' ' << value.z << endl;
                        }
                    }
                }
            }
            sceneFile << "EOF" << endl; // End of fragment shader uniforms

            for (unsigned int j = 0; j < model->instanceCount; j++)
            {
                sceneFile << model->names[j] << endl;
                sceneFile << model->Hash_ID[j] << endl;

                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile << model->transforms[j].position[k] << ' ';
                }
                sceneFile << endl;
                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile << model->transforms[j].rotation[k] << ' ';
                }
                sceneFile << endl;
                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile << model->transforms[j].scale[k] << ' ';
                }
                sceneFile << endl;
            }
            
        }

        */
        sceneFile.close();
    }
}

void loadScene()
{
    ifstream sceneFile;
    sceneFile.open("localData/scene.sn");

    if (sceneFile.is_open())
    {
        /*
        unsigned int numModels;
        sceneFile >> numModels;
        for (int i = 1; i < numModels; i++)
        {
            string path;
            sceneFile >> path;
            unsigned int instanceCount;
            sceneFile >> instanceCount;
            string vertexShaderPath;
            sceneFile >> vertexShaderPath;
            string fragmentShaderPath;
            sceneFile >> fragmentShaderPath;

            //we need to load the shader properties
            //so parse the shader files, get the properties name, type and value. then load
            bool foundEOV = false;

            vector<string> names;
            vector<string> types;

            vector<int> ints;
            vector<float> floats;
            vector<glm::vec3> vec3s;

            while (true)
            {
                string name;
                sceneFile >> name;
                if (name == "EOV")
                {
                    break; // End of vertex shader uniforms
                }

                string type;
                sceneFile >> type;

                names.push_back(name);
                types.push_back(type);

                if (type == "int")
                {
                    int intValue;
                    sceneFile >> intValue;
                    ints.push_back(intValue);
                }
                else if (type == "float")
                {
                    float floatValue;
                    sceneFile >> floatValue;
                    floats.push_back(floatValue);
                }
                else if (type == "vec3")
                {
                    glm::vec3 vec3Value;
                    sceneFile >> vec3Value.x >> vec3Value.y >> vec3Value.z;
                    vec3s.push_back(vec3Value);
                }
            }

            while (true)
            {
                string name;
                sceneFile >> name;
                if (name == "EOF")
                {
                    break; // End of fragment shader uniforms
                }

                string type;
                sceneFile >> type;

                names.push_back(name);
                types.push_back(type);

                if (type == "int")
                {
                    int intValue;
                    sceneFile >> intValue;
                    ints.push_back(intValue);
                }
                else if (type == "float")
                {
                    float floatValue;
                    sceneFile >> floatValue;
                    floats.push_back(floatValue);
                }
                else if (type == "vec3")
                {
                    glm::vec3 vec3Value;
                    sceneFile >> vec3Value.x >> vec3Value.y >> vec3Value.z;
                    vec3s.push_back(vec3Value);
                }
            }


            string name;
            sceneFile >> name;

            size_t hashID;
            sceneFile >> hashID;

            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;

            for (unsigned int k = 0; k < 3; k++)
            {
                sceneFile >> position[k];
            }
            for (unsigned int k = 0; k < 3; k++)
            {
                sceneFile >> rotation[k];
            }
            for (unsigned int k = 0; k < 3; k++)
            {
                sceneFile >> scale[k];
            }

            Model *model = new Model(path.c_str(), vertexShaderPath.c_str(), fragmentShaderPath.c_str(), name, position, rotation, scale);
            
            int intCount = 0;
            int floatCount = 0;
            int vec3Count = 0;

            model->shader->use();

            for(int i = 0; i < names.size(); i++)
            {
                if (types[i] == "int")
                {
                    model->shader->setInt(names[i].c_str(), ints[intCount++]);
                }
                else if (types[i] == "float")
                {
                    model->shader->setFloat(names[i].c_str(), floats[floatCount++]);
                }
                else if (types[i] == "vec3")
                {
                    model->shader->setVec3(names[i].c_str(), vec3s[vec3Count++]);
                }
            }

            model->Hash_ID[0] = hashID;

            //sceneModels.push_back(model);

            SceneTreeNode *sceneNode;
            //sceneNode = insertInstanceToSceneTree(rootNode, model, 0);
            sceneRootNode->childrenInstances.push_back(sceneNode);
            sceneNode->parentNode = sceneRootNode;

            cout << "loaded model with path: " << path << endl;


            for (unsigned int j = 1; j < instanceCount; j++)
            {
                string name;
                sceneFile >> name;

                size_t hashID;
                sceneFile >> hashID;

                glm::vec3 position;
                glm::vec3 rotation;
                glm::vec3 scale;

                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile >> position[k];
                }
                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile >> rotation[k];
                }
                for (unsigned int k = 0; k < 3; k++)
                {
                    sceneFile >> scale[k];
                }
                
                model->names[i] = name;
                model->Hash_ID[i] = hashID;
                model->transforms[i] = Transform{position, rotation, scale};

                int index = model->addInstance(position, rotation, scale, name);

                SceneTreeNode *sceneNode;
                //sceneNode = insertInstanceToSceneTree(rootNode, model, j);
                sceneRootNode->childrenInstances.push_back(sceneNode);
                sceneNode->parentNode = sceneRootNode;
            }
        }*/
    }
}

void saveData()
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

        if(SCR_HEIGHT == 1) SCR_HEIGHT = 720;
        if(SCR_WIDTH == 1) SCR_WIDTH = 1280;
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

        saveFile.close();
    }
}

void loadData()
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

        saveFile.close();
    }
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



    currentPath = fs::current_path();
}

void drawMainUI(){
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

    if (ImGui::Button("Save Data"))
    {
        saveData();
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
    ImGui::End();
}

void ShowFileBrowser()
{
    ImGui::Begin("File Browser");

    // Back button
    if (ImGui::Button("⬅ Up") && currentPath.has_parent_path()) {
        currentPath = currentPath.parent_path();
    }

    ImGui::Separator();
    ImGui::Text("Current Path: %s", currentPath.string().c_str());
    ImGui::Separator();
    std::string filename;

    // Iterate through directory
    for (auto& entry : fs::directory_iterator(currentPath)) {
        const auto& path = entry.path();
        filename = path.filename().string();

        if (entry.is_directory()) {
            // Folders: click to enter
            if (ImGui::Selectable((filename + "/").c_str(), false, ImGuiSelectableFlags_AllowDoubleClick)) {
                if (ImGui::IsMouseDoubleClicked(0)) {
                    currentPath = path;
                }
            }
        } else {
            // Files: click to select
            if (ImGui::Selectable(filename.c_str(), selectedFile == filename)) {
                selectedFile = filename;
                cout << "Selected file path: " << currentPath << '\\' << filename << endl;
            }
        }
        
    }

    if (ImGui::Button("Spawn Model?")){
        if (selectedNode != sceneRootNode)
        {
            Transform transform{camera.Position + camera.Front * 2.0f, glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};
            createNewModel((currentPath / selectedFile).string().c_str(), "resources/shaders/missingShader_vertex.glsl", "resources/shaders/missingShader_fragment.glsl", selectedFile, transform, selectedNode);
        }
    }

    if (ImGui::Button("reload shaders")){
        for (int i = 0; i < HASH_TABLE_SIZE; i++)
        {
            for (int j = 0; j < hashTable[i].size(); j++)
            {
                hashTable[i][j]->NodeModel->reloadShader();
            }
        }
    }
    ImGui::End();
}

void drawSceneTreeHierarchical(SceneTreeNode* node, SceneTreeNode*& selectedNode)
{

    //ok so first start from the root node
    //if the dropdown is open, then we need to draw the children
    //if any of those children are selected, we need to draw them as well

    // label for this node
    std::string label = node->name;

    // is this node selected?
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
                               ImGuiTreeNodeFlags_OpenOnDoubleClick |
                               (node->childrenInstances.empty() ? ImGuiTreeNodeFlags_Leaf : 0) |
                               ((selectedNode == node) ? ImGuiTreeNodeFlags_Selected : 0);

    bool open = ImGui::TreeNodeEx((void*)node, flags, "%s", label.c_str());

    // handle selection
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        selectedNode = node;

    // draw children recursively
    if (open)
    {
        for (SceneTreeNode* child : node->childrenInstances)
            drawSceneTreeHierarchical(child, selectedNode);

        ImGui::TreePop();
    }
}

void drawSceneTree(){
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
        ImGui::Text("Hash ID: %zu", selectedNode->hashID);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;

    
        static int selected = -1;
        if (ImGui::CollapsingHeader("Mesh Selection:", ImGuiTreeNodeFlags_None))
        {
            int verticalSize = 0;
            if (selectedNode->NodeModel->meshes.size() > 7)
                verticalSize = 160;
            else
                verticalSize = selectedNode->NodeModel->meshes.size() * 15;
            
            ImGui::BeginChild("ChildL", ImVec2(ImGui::GetContentRegionAvail().x * 0.75, verticalSize), ImGuiChildFlags_None, window_flags);
            for (int i = 0; i < selectedNode->NodeModel->meshes.size(); i++)
            {
                char buf[32];
                sprintf(buf, "Mesh %d", i);
                if (ImGui::Selectable(buf, selected == i))
                    selected = i;
            }
            ImGui::EndChild();
        }

        if (ImGui::CollapsingHeader("Set Shader properties:", ImGuiTreeNodeFlags_None))
        {
            char vertexShaderBuffer[512];
            char fragmentShaderBuffer[512];

            std::snprintf(vertexShaderBuffer, sizeof(vertexShaderBuffer), "%s", selectedNode->NodeModel->vertexShaderPath.c_str());
            std::snprintf(fragmentShaderBuffer, sizeof(fragmentShaderBuffer), "%s", selectedNode->NodeModel->fragmentShaderPath.c_str());

            if (ImGui::InputText("Vertex Shader", vertexShaderBuffer, IM_ARRAYSIZE(vertexShaderBuffer)))
            {
                selectedNode->NodeModel->vertexShaderPath = vertexShaderBuffer;
            }

            if (ImGui::InputText("Fragment Shader", fragmentShaderBuffer, IM_ARRAYSIZE(fragmentShaderBuffer)))
            {
                selectedNode->NodeModel->fragmentShaderPath = fragmentShaderBuffer;
            }

            // lets try to parse the shaders so we can set values easier
            ifstream vertexShaderFile;
            ifstream fragmentShaderFile;
            vertexShaderFile.open(selectedNode->NodeModel->vertexShaderPath);
            fragmentShaderFile.open(selectedNode->NodeModel->fragmentShaderPath);

            if (selectedNode != previousSelectedNode)
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

                previousSelectedNode = selectedNode;
            }

            if (!alreadyCreated)
            {
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
                for (string line; getline(fragmentShaderFile, line);)
                {
                    if (line.find("uniform") != string::npos)
                    {
                        // get data type and name
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

                // alreadyCreated = true;
            }
            ImGui::Separator();

            ImGui::Text("Vertex Shader Uniforms:");

            int intCounter = 0;
            int floatCounter = 0;
            int vec3Counter = 0;

            for (int i = 0; i < vertexUniforms.size(); i++)
            {
                ImGui::Text("%s: ", vertexUniforms[i].c_str());
                if (vertexUniformTypes[i] == "float")
                {

                    if (!alreadyCreated)
                        vertexUniformFloats.push_back(selectedNode->NodeModel->shader->getFloat(vertexUniforms[i].c_str()));
                    if (ImGui::SliderFloat(vertexUniforms[i].c_str(), &vertexUniformFloats[floatCounter], 0.0f, 1.0f))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setFloat(vertexUniforms[i].c_str(), vertexUniformFloats[floatCounter]);
                    }
                    floatCounter++;
                }
                else if (vertexUniformTypes[i] == "vec3")
                {
                    if (!alreadyCreated)
                        vertexUniformVec3s.push_back(selectedNode->NodeModel->shader->getVec3(vertexUniforms[i].c_str()));
                    if (ImGui::SliderFloat3(vertexUniforms[i].c_str(), &vertexUniformVec3s[vec3Counter][0], 0.0f, 1.0f))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setVec3(vertexUniforms[i].c_str(), vertexUniformVec3s[vec3Counter][0], vertexUniformVec3s[vec3Counter][1], vertexUniformVec3s[vec3Counter][2]);
                    }
                    vec3Counter++;
                }
                else if (vertexUniformTypes[i] == "int")
                {
                    if (!alreadyCreated)
                        vertexUniformInts.push_back(selectedNode->NodeModel->shader->getInt(vertexUniforms[i].c_str()));
                    if (ImGui::SliderInt(vertexUniforms[i].c_str(), &vertexUniformInts[intCounter], 0, 100))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setInt(vertexUniforms[i].c_str(), vertexUniformInts[intCounter]);
                    }
                    intCounter++;
                }
            }
            ImGui::Text("Fragment Shader Uniforms:");

            floatCounter = 0;
            vec3Counter = 0;
            intCounter = 0;

            for (int i = 0; i < fragmentUniforms.size(); i++)
            {
                ImGui::Text("%s: ", fragmentUniforms[i].c_str());
                if (fragmentUniformTypes[i] == "float")
                {
                    if (!alreadyCreated)
                        fragmentUniformFloats.push_back(selectedNode->NodeModel->shader->getFloat(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderFloat(fragmentUniforms[i].c_str(), &fragmentUniformFloats[floatCounter], 0.0f, 1.0f))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setFloat(fragmentUniforms[i].c_str(), fragmentUniformFloats[floatCounter]);
                    }
                    floatCounter++;
                }
                else if (fragmentUniformTypes[i] == "vec3")
                {
                    if (!alreadyCreated)
                        fragmentUniformVec3s.push_back(selectedNode->NodeModel->shader->getVec3(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderFloat3(fragmentUniforms[i].c_str(), &fragmentUniformVec3s[vec3Counter][0], 0.0f, 1.0f))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setVec3(fragmentUniforms[i].c_str(), fragmentUniformVec3s[vec3Counter][0], fragmentUniformVec3s[vec3Counter][1], fragmentUniformVec3s[vec3Counter][2]);
                    }
                    vec3Counter++;
                }
                else if (fragmentUniformTypes[i] == "int")
                {
                    if (!alreadyCreated)
                        fragmentUniformInts.push_back(selectedNode->NodeModel->shader->getInt(fragmentUniforms[i].c_str()));
                    if (ImGui::SliderInt(fragmentUniforms[i].c_str(), &fragmentUniformInts[intCounter], 0, 100))
                    {
                        selectedNode->NodeModel->shader->use();
                        selectedNode->NodeModel->shader->setInt(fragmentUniforms[i].c_str(), fragmentUniformInts[intCounter]);
                    }
                    intCounter++;
                }
            }

            if (!alreadyCreated)
            {
                alreadyCreated = true;
            }
        }
        ImGui::Separator();

        if (ImGui::Button("Reload Selected Shader"))
        {
            selectedNode->NodeModel->reloadShader();
        }

        if (ImGui::Button("Delete"))
        {
            SceneTreeNode* nodeToDelete = selectedNode;
            SceneTreeNode* parentNode = nodeToDelete->parentNode;

            removeNodeFromSceneTree(nodeToDelete);

            selectedNode = nullptr;

        }

        if (ImGui::Button("set test Image in shader")){
            if (runtimeTestTextureID == 0)
            {
                runtimeTestTextureID = selectedNode->NodeModel->TextureFromFile("resources/textures/awesomeface.png", "", false);;
            }

            runtimeTextureTargetNode = selectedNode;
            selectedNode->NodeModel->shader->use();
            glActiveTexture(GL_TEXTURE0 + runtimeTestTextureUnit);
            glBindTexture(GL_TEXTURE_2D, runtimeTestTextureID);
            selectedNode->NodeModel->shader->setInt("testTexture", runtimeTestTextureUnit);
        }
    }

    ImGui::End();
}

void drawAllUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainUI();
   
    drawSceneTree();
    ShowFileBrowser();
    //drawSceneTreeHierarchical(sceneRootNode, selectedNode);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#endif