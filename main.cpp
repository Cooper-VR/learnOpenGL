#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <vector>
#include <stack>
#include <shaders/shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera/camera.hpp>
#include <model/model.hpp>
#include <imgui/imgui.h>
#include <helpers/sceneTree.hpp>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>

using namespace std;
namespace fs = std::filesystem;

void drawSceneTreeHierarchical(SceneTreeNode* node, SceneTreeNode*& selectedNode);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLFWwindow* setupOpenGL();
void setUpImGui(GLFWwindow* window);
void ShowFileBrowser();
void drawAllUI();
void drawMainUI();
void drawSceneTree();
void saveScene();
void loadScene();
void saveData();
void loadData();
void resetData();

float cameraFOV = 45.0f;
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool mousePressLeft = false;
bool mousePressRight = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

static int numberOfVertices = 0;
static int numberOfBatches = 0;

float RotateSensitivity = 1.0f;
float PanSensitivity = 1.0f;
float ForwardSensitivity = 1.0f;

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

vector<Model *> sceneModels;
SceneTreeNode *rootNode;
SceneTreeNode *sceneRootNode;

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

    string path = "resources/models/testCube.fbx";

    rootNode = new SceneTreeNode();
    rootNode->NodeModel = nullptr;
    rootNode->instanceCount = 0;
    rootNode->leftChildInstance = nullptr;
    rootNode->rightChildInstance = nullptr;
    rootNode->parentNode = nullptr;
    
    string fragment = "resources/shaders/objectLighting_fragment.glsl";
    string vertex = "resources/shaders/objectLighting_vertex.glsl";
    Model* test = new Model(path.c_str(), vertex.c_str(), fragment.c_str(), "rootObject");
    test->transforms[0] = Transform{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};

    sceneRootNode = insertInstanceToSceneTree(rootNode, test, 0);
    
    sceneModels.push_back(test);

    //loadScene();

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

        for (int i = 0; i < sceneModels.size(); i++)
        {
            Model *model = sceneModels[i];
            numberOfVertices += model->numberOfVertices;
            numberOfBatches += model->numberOfBatches;
            model->numberOfVertices = 0;
            model->numberOfBatches = 0;
            model->shader->use();
            model->shader->setVec3("viewPos", camera.Position);
            //set view pos for shader editor part too
            for (int j = 0; j < vertexUniforms.size(); j++)
            {
                if (vertexUniforms[j] == "viewPos")
                {
                    vertexUniformVec3s[j] = camera.Position;
                    model->shader->setVec3(vertexUniforms[j].c_str(), vertexUniformVec3s[j]);
                }
            }
            for (int j = 0; j < fragmentUniforms.size(); j++)
            {
                if (fragmentUniforms[j] == "viewPos")
                {
                    fragmentUniformVec3s[j] = camera.Position;
                    model->shader->setVec3(fragmentUniforms[j].c_str(), fragmentUniformVec3s[j]);
                }
            }

            model->shader->setVec3("dirLight.direction", dirLightDirection[0], dirLightDirection[1], dirLightDirection[2]);
            model->shader->setVec3("dirLight.ambient", dirLightAmbientColor[0], dirLightAmbientColor[1], dirLightAmbientColor[2]);
            model->shader->setVec3("dirLight.diffuse", dirLightDiffuseColor[0], dirLightDiffuseColor[1], dirLightDiffuseColor[2]);
            model->shader->setVec3("dirLight.specular", dirLightSpecularColor[0], dirLightSpecularColor[1], dirLightSpecularColor[2]);
            model->Draw(projection, view);
        }

        drawAllUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
        numberOfVertices = 0;
        numberOfBatches = 0;
    }

    cout << "closing application" << endl;

    for (unsigned int i = 0; i < sceneModels.size(); i++)
    {
        delete sceneModels[i];
    }

    saveData();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

GLFWwindow* setupOpenGL(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);  

    return window;
}

void setUpImGui(GLFWwindow* window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

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

        sceneFile << sceneModels.size() << endl;
        for (int i = 1; i < sceneModels.size(); i++)
        {
            Model *model = sceneModels[i];
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

        sceneFile.close();
    }
}

void loadScene()
{
    ifstream sceneFile;
    sceneFile.open("localData/scene.sn");

    if (sceneFile.is_open())
    {
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
                cout << "Loading uniform: " << names[i] << " of type: " << types[i] << " with value: ";
                if (types[i] == "int")
                {
                    model->shader->setInt(names[i].c_str(), ints[intCount++]);
                    cout << ints[intCount-1] << endl;
                }
                else if (types[i] == "float")
                {
                    model->shader->setFloat(names[i].c_str(), floats[floatCount++]);
                    cout << floats[floatCount-1] << endl;
                }
                else if (types[i] == "vec3")
                {
                    model->shader->setVec3(names[i].c_str(), vec3s[vec3Count++]);
                    cout << vec3s[vec3Count-1].x << ' ' << vec3s[vec3Count-1].y << ' ' << vec3s[vec3Count-1].z << endl;
                }
            }

            model->Hash_ID[0] = hashID;

            sceneModels.push_back(model);

            SceneTreeNode *sceneNode;
            sceneNode = insertInstanceToSceneTree(rootNode, model, 0);
            cout << "Inserted model with Hash ID: " << sceneNode->NodeModel->Hash_ID[0] << endl;
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
                sceneNode = insertInstanceToSceneTree(rootNode, model, j);
                cout << "Inserted model with Hash ID: " << sceneNode->NodeModel->Hash_ID[j] << endl;
                sceneRootNode->childrenInstances.push_back(sceneNode);
                sceneNode->parentNode = sceneRootNode;
            }
        }
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

void processInput(GLFWwindow *window)
{

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        // nothing
    }

    /*
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
        */
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height){
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_HEIGHT = height;
    SCR_WIDTH = width;
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (!ImGui::GetIO().WantCaptureMouse)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            camera.PanCamera(-xoffset * PanSensitivity, yoffset * PanSensitivity, deltaTime);
        }
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) != GLFW_PRESS)
    {
        return;
        xoffset = ypos = 0.0f;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        // camera rotation
        if (!mousePressRight)
            xoffset = yoffset = 0.0f;
        mousePressRight = true;
    }
    else
    {
        mousePressRight = false;
    }

    camera.RotateCamera(xoffset * RotateSensitivity, yoffset * RotateSensitivity, false);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.MoveCameraForward(static_cast<float>(yoffset) * ForwardSensitivity);
}

void drawAllUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainUI();
    drawSceneTree();
    ShowFileBrowser();
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void drawMainUI(){
    ImGui::Begin("OpenGL UI");
    ImGui::Text("FPS: %.1f", deltaTime != 0.0f ? (1.0f / deltaTime) : 0.0f);
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
}

void drawSceneTree(){
    ImGui::Begin("Scene Tree");

    static SceneTreeNode *selectedNode = nullptr;
    static SceneTreeNode *previousSelectedNode = nullptr;
    drawSceneTreeHierarchical(rootNode, selectedNode);

    if (selectedNode)
    {
        ImGui::Separator();
        ImGui::Text("Selected Model: %s", selectedNode->NodeModel->names[selectedNode->instanceCount].c_str());
        ImGui::DragFloat3("Position", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].position), 0.001f);
        ImGui::DragFloat3("Rotation", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].rotation), 1.0f);
        ImGui::DragFloat3("Scale", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].scale), 0.1f, 0.1f, 10.0f);
        ImGui::Text("Hash ID: %zu", selectedNode->NodeModel->Hash_ID[selectedNode->instanceCount]);

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

        //lets try to parse the shaders so we can set values easier
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

                    //cout << "Found vertex shader uniform: " << vertexUniforms.back() << " of type " << vertexUniformTypes.back() << endl;
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

                    //cout << "Found fragment shader uniform: " << fragmentUniforms.back() << " of type " << fragmentUniformTypes.back() << endl;
                }
            }
            
            //alreadyCreated = true;
        }
        ImGui::Separator();

        ImGui::Text("Set Shader properties:");
        ImGui::Text("Vertex Shader Uniforms:");

        int intCounter = 0;
        int floatCounter = 0;
        int vec3Counter = 0;

        for (int i = 0; i < vertexUniforms.size(); i++)
        {
            ImGui::Text("%s: ", vertexUniforms[i].c_str());
            if (vertexUniformTypes[i] == "float")
            {

                if (!alreadyCreated) vertexUniformFloats.push_back(selectedNode->NodeModel->shader->getFloat(vertexUniforms[i].c_str()));
                if (ImGui::SliderFloat(vertexUniforms[i].c_str(), &vertexUniformFloats[floatCounter], 0.0f, 1.0f))
                {
                    selectedNode->NodeModel->shader->use();
                    selectedNode->NodeModel->shader->setFloat(vertexUniforms[i].c_str(), vertexUniformFloats[floatCounter]);
                }
                floatCounter++;
            }
            else if (vertexUniformTypes[i] == "vec3")
            {
                if (!alreadyCreated) vertexUniformVec3s.push_back(selectedNode->NodeModel->shader->getVec3(vertexUniforms[i].c_str()));
                if (ImGui::SliderFloat3(vertexUniforms[i].c_str(), &vertexUniformVec3s[vec3Counter][0], 0.0f, 1.0f))
                {
                    selectedNode->NodeModel->shader->use();
                    selectedNode->NodeModel->shader->setVec3(vertexUniforms[i].c_str(), vertexUniformVec3s[vec3Counter][0], vertexUniformVec3s[vec3Counter][1], vertexUniformVec3s[vec3Counter][2]);
                }
                vec3Counter++;
            }
            else if (vertexUniformTypes[i] == "int")
            {
                if (!alreadyCreated) vertexUniformInts.push_back(selectedNode->NodeModel->shader->getInt(vertexUniforms[i].c_str()));
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
                if (!alreadyCreated) fragmentUniformFloats.push_back(selectedNode->NodeModel->shader->getFloat(fragmentUniforms[i].c_str()));
                if (ImGui::SliderFloat(fragmentUniforms[i].c_str(), &fragmentUniformFloats[floatCounter], 0.0f, 1.0f))
                {
                    selectedNode->NodeModel->shader->use();
                    selectedNode->NodeModel->shader->setFloat(fragmentUniforms[i].c_str(), fragmentUniformFloats[floatCounter]);
                }
                floatCounter++;
            }
            else if (fragmentUniformTypes[i] == "vec3")
            {
                if (!alreadyCreated) fragmentUniformVec3s.push_back(selectedNode->NodeModel->shader->getVec3(fragmentUniforms[i].c_str()));
                if (ImGui::SliderFloat3(fragmentUniforms[i].c_str(), &fragmentUniformVec3s[vec3Counter][0], 0.0f, 1.0f))
                {
                    selectedNode->NodeModel->shader->use();
                    selectedNode->NodeModel->shader->setVec3(fragmentUniforms[i].c_str(), fragmentUniformVec3s[vec3Counter][0], fragmentUniformVec3s[vec3Counter][1], fragmentUniformVec3s[vec3Counter][2]);
                }
                vec3Counter++;
            }
            else if (fragmentUniformTypes[i] == "int")
            {
                if (!alreadyCreated) fragmentUniformInts.push_back(selectedNode->NodeModel->shader->getInt(fragmentUniforms[i].c_str()));
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

        ImGui::Separator();

        if (ImGui::Button("Reload Selected Shader"))
        {
            selectedNode->NodeModel->reloadShader();
        }

        if (ImGui::Button("Delete"))
        {
            SceneTreeNode* nodeToDelete = selectedNode;
            SceneTreeNode* parentNode = nodeToDelete->parentNode;
            std::string instanceName = nodeToDelete->NodeModel->names[nodeToDelete->instanceCount];

            selectedNode = nullptr;
            removeInstanceFromSceneTreeByName(rootNode, nodeToDelete->NodeModel, instanceName);

            if (parentNode)
            {
                auto &siblings = parentNode->childrenInstances;
                siblings.erase(std::remove(siblings.begin(), siblings.end(), nodeToDelete), siblings.end());
            }

            sceneModels.erase(std::remove(sceneModels.begin(), sceneModels.end(), nodeToDelete->NodeModel), sceneModels.end());
        }
    }

    ImGui::End();
}

void drawSceneTreeHierarchical(SceneTreeNode* node, SceneTreeNode*& selectedNode)
{
    if (!node || !node->NodeModel)
        return;

    // label for this node
    std::string label = node->NodeModel->names[node->instanceCount];

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
        bool alreadyLoaded = false;
        SceneTreeNode *sceneNode;

        //check if the selected file is already a loeaded model
        for (Model* model : sceneModels) {
            cout << "Checking loaded model: " << model->directory << endl;
            if (model->directory == (currentPath / selectedFile).string()) {
                alreadyLoaded = true;
                cout << "Model already loaded: " << model->directory << endl;
                int index = model->addInstance(camera.Position + camera.Front * 2.0f, glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), selectedFile);
                sceneNode = insertInstanceToSceneTree(rootNode, model, index);
                cout << "Inserted model with Hash ID: " << sceneNode->NodeModel->Hash_ID[index] << endl;
                sceneRootNode->childrenInstances.push_back(sceneNode);
                sceneNode->parentNode = sceneRootNode;
                break;
            }
        }

        string fragment = "resources/shaders/missingShader_fragment.glsl";
        string vertex = "resources/shaders/missingShader_vertex.glsl";
        if (!alreadyLoaded){
            cout << "Loading model from: " << (currentPath / selectedFile).string() << endl;
            Model *newModel = new Model((currentPath / selectedFile).string().c_str(), vertex.c_str(), fragment.c_str(), selectedFile);
            newModel->transforms[0].position = camera.Position + camera.Front * 2.0f;
            newModel->transforms[0].scale = glm::vec3(0.2f, 0.2f, 0.2f);
            newModel->transforms[0].rotation = glm::vec3(-90.0f, 0.0f, 0.0f);

            sceneNode = insertInstanceToSceneTree(rootNode, newModel, 0);
            cout << "Inserted model with Hash ID: " << sceneNode->NodeModel->Hash_ID[0] << endl;
            sceneRootNode->childrenInstances.push_back(sceneNode);
            sceneModels.push_back(newModel);
            sceneNode->parentNode = sceneRootNode;
        }
        
    }

    if (ImGui::Button("reload shaders")){
        for (Model* model : sceneModels) {
            model->reloadShader();
        }
    }
    ImGui::End();
}