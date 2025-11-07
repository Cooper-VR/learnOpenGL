#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <fstream>
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
void ShowFileBrowser();
void saveScene();
void loadScene();
void saveData();
void loadData();
void resetData();

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

float RotateSensitivity = 1.0f;
float PanSensitivity = 1.0f;
float ForwardSensitivity = 1.0f;

float skyColor[3]{0.4f, 0.4f, 0.9f};
float dirLightDiffuseColor[3] = {0.5f, 0.5f, 0.5f};
float dirLightAmbientColor[3] = {0.2f, 0.2f, 0.2f};
float dirLightSpecularColor[3] = {1.0f, 1.0f, 1.0f};
float lightAmbientColor[3] = {0.05f, 0.05f, 0.05f};
float lightDiffuseColor[3] = {0.8f, 0.8f, 0.8f};
float lightSpecularColor[3] = {1.0f, 1.0f, 1.0f};
float lightLinear = 0.09f;
float lightQuatratic = 0.032f;

string pointLightAttribs[7] = {
    ".position",
    ".ambient",
    ".diffuse",
    ".specular",
    ".constant",
    ".linear",
    ".quadratic"};

static fs::path currentPath = fs::current_path();
static std::string selectedFile = "";

vector<Model *> sceneModels;
SceneTreeNode *rootNode;
SceneTreeNode *sceneRootNode;
int main()
{
    loadData();
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
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
        return -1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_DEPTH_TEST);

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    string path = "resources/models/champion.fbx";

    rootNode = new SceneTreeNode{nullptr, 0, nullptr, nullptr};
    
    string fragment = "resources/shaders/objectLighting_fragment.glsl";
    string vertex = "resources/shaders/objectLighting_vertex.glsl";
    Model* test = new Model(path.c_str(), vertex.c_str(), fragment.c_str(), "champion");
    test->transforms[0] = Transform{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};

    sceneRootNode = insertInstanceToSceneTree(rootNode, test, 0);

    cout << "Inserted model with Hash ID: " << sceneRootNode->NodeModel->Hash_ID[sceneRootNode->instanceCount] << endl;

    path = "resources/models/testCube.fbx";
    fragment = "resources/shaders/litObject_fragment.glsl";
    vertex = "resources/shaders/litObject_vertex.glsl";
    Model* cubeModel = new Model(path.c_str(), vertex.c_str(), fragment.c_str(), "cube");
    cubeModel->transforms[0] = Transform{pointLightPositions[0], glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f)};


    SceneTreeNode *sceneLightNode = insertInstanceToSceneTree(rootNode, cubeModel, 0);
    cout << "Inserted model with Hash ID: " << cubeModel->Hash_ID[0] << endl;
    sceneRootNode->childrenInstances.push_back(sceneLightNode);
    sceneLightNode->parentNode = sceneRootNode;

    SceneTreeNode* sceneNode;

    for (unsigned int i = 1; i < 4; i++)
    {
        int index = cubeModel->addInstance(pointLightPositions[i], glm::vec3(-90.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f), "lightCube " + std::to_string(i));
        sceneNode = insertInstanceToSceneTree(rootNode, cubeModel, i);
        cout << "Inserted model with Hash ID: " << sceneNode->NodeModel->Hash_ID[i] << endl;
        sceneLightNode->childrenInstances.push_back(sceneNode);
        sceneNode->parentNode = sceneLightNode;
    }

    sceneModels.push_back(test);
    sceneModels.push_back(cubeModel);

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

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("OpenGL UI");
        ImGui::Text("FPS: %.1f", deltaTime != 0.0f ? (1.0f / deltaTime) : 0.0f);

        ImGui::SliderFloat("RotateSensitivity", &RotateSensitivity, 0.1f, 5.0f);
        ImGui::SliderFloat("PanSensitivity", &PanSensitivity, 0.1f, 5.0f);
        ImGui::SliderFloat("ForwardSensitivity", &ForwardSensitivity, 0.1f, 5.0f);

        ImGui::ColorEdit3("SkyColor", skyColor);
        ImGui::ColorEdit3("DirLightDiffuseColor", dirLightDiffuseColor);
        ImGui::ColorEdit3("DirLightAmbientColor", dirLightAmbientColor);
        ImGui::ColorEdit3("DirLightSpecularColor", dirLightSpecularColor);

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

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        glm::mat4 view = camera.GetViewMatrix();

        if (ImGui::Button("Save Data"))
        {
            saveData();
        }
        if (ImGui::Button("reset save data"))
        {
            resetData();
        }

        Model *model = sceneModels[0];
        model->shader->use();
        model->shader->setVec3("ViewDir", camera.Position);
        model->shader->setFloat("material.shininess", 0.0f);
        model->shader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        model->shader->setVec3("dirLight.ambient", dirLightAmbientColor[0], dirLightAmbientColor[1], dirLightAmbientColor[2]);
        model->shader->setVec3("dirLight.diffuse", dirLightDiffuseColor[0], dirLightDiffuseColor[1], dirLightDiffuseColor[2]);
        model->shader->setVec3("dirLight.specular", dirLightSpecularColor[0], dirLightSpecularColor[1], dirLightSpecularColor[2]);

        model->shader->setVec3("spotLight.position", camera.Position);
        model->shader->setVec3("spotLight.direction", camera.Front);
        model->shader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        model->shader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        model->shader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        model->shader->setFloat("spotLight.constant", 1.0f);
        model->shader->setFloat("spotLight.linear", 0.09f);
        model->shader->setFloat("spotLight.quadratic", 0.032f);
        model->shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        model->shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        ImGui::Text("lights");
        ImGui::ColorEdit3("LightAmbientColor", lightAmbientColor);
        ImGui::ColorEdit3("LightDiffuseColor", lightDiffuseColor);
        ImGui::ColorEdit3("LightSpecularColor", lightSpecularColor);
        ImGui::DragFloat("light linear", &lightLinear, 0.01, 0.01);
        ImGui::DragFloat("light quadratic", &lightQuatratic, 0.005, 0.05);

        for (int i = 0; i < 4; i++)
        {
            pointLightPositions[i].x = cubeModel->transforms[i].position.x;
            pointLightPositions[i].y = cubeModel->transforms[i].position.y;
            pointLightPositions[i].z = cubeModel->transforms[i].position.z;

            model->shader->setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[0], pointLightPositions[0]);
            model->shader->setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[1], lightAmbientColor[0], lightAmbientColor[1], lightAmbientColor[2]);
            model->shader->setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[2], lightDiffuseColor[0], lightDiffuseColor[1], lightDiffuseColor[2]);
            model->shader->setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[3], lightSpecularColor[0], lightSpecularColor[1], lightSpecularColor[2]);
            model->shader->setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[4], 1.0f);
            model->shader->setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[5], lightLinear);
            model->shader->setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[6], lightQuatratic);
        }

        model = sceneModels[1];
        for (unsigned int i = 0; i < 1; i++)
        {

            model->shader->use();
            model->shader->setVec3("mainColor", glm::vec3(lightDiffuseColor[0], lightDiffuseColor[1], lightDiffuseColor[2]));
        }

        for (int i = 0; i < sceneModels.size(); i++)
        {
            Model *model = sceneModels[i];

            model->Draw(projection, view);
        }


        ImGui::Begin("Scene Tree");

        static SceneTreeNode *selectedNode = nullptr;
        drawSceneTreeHierarchical(rootNode, selectedNode);

        if (selectedNode)
        {
            ImGui::Separator();
            ImGui::Text("Selected Model: %s", selectedNode->NodeModel->names[selectedNode->instanceCount].c_str());
            ImGui::DragFloat3("Position", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].position), 0.1f);
            ImGui::DragFloat3("Rotation", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].rotation), 1.0f);
            ImGui::DragFloat3("Scale", glm::value_ptr(selectedNode->NodeModel->transforms[selectedNode->instanceCount].scale), 0.1f, 0.1f, 10.0f);
        }

        ImGui::End();

        ShowFileBrowser();


        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup

    cout << "closing application" << endl;

    delete cubeModel;
    delete test;
    // get the imguiu stuff store it in a file
    saveData();
    saveScene();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

void saveScene()
{
    // save the scene to a file
    ofstream sceneFile;
    sceneFile.open("localData/scene.sn");
    if (sceneFile.is_open())
    {
        for (int i = 0; i < sceneModels.size(); i++)
        {
            Model *model = sceneModels[i];
            sceneFile << model->directory << endl;
            sceneFile << model->instanceCount << endl;

            for (unsigned int j = 0; j < model->instanceCount; j++)
            {

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

    /*
    loop through each model,
        save the path, number of instances
        loop though for the number of instances
            save the name, position, rotation, scale, hash id
    */
}

void loadScene()
{
    ifstream sceneFile;
    sceneFile.open("localData/scene.sn");

    if (sceneFile.is_open())
    {
        string path;
        sceneFile >> path;
        unsigned int numInstances;
        sceneFile >> numInstances;

        for (unsigned int i = 0; i < numInstances; i++)
        {
            glm::vec3 position;
            glm::vec3 rotation;
            glm::vec3 scale;
            size_t hashID;
            sceneFile >> hashID;
            sceneFile >> position.x >> position.y >> position.z;
            sceneFile >> rotation.x >> rotation.y >> rotation.z;
            sceneFile >> scale.x >> scale.y >> scale.z;


        }
        
    }
    /*
    loop through each model,
        load the path, number of instances
        loop though for the number of instances
            load the name, position, rotation, scale, hash id
    */
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

        for (int i = 0; i < 3; i++)
            saveFile << lightAmbientColor[i] << ' ';
        saveFile << endl;
        for (int i = 0; i < 3; i++)
            saveFile << lightDiffuseColor[i] << ' ';
        saveFile << endl;
        for (int i = 0; i < 3; i++)
            saveFile << lightSpecularColor[i] << ' ';

        saveFile << endl;
        saveFile << lightLinear << endl;
        saveFile << lightQuatratic << endl;

        if(SCR_HEIGHT == 1) SCR_HEIGHT = 720;
        if(SCR_WIDTH == 1) SCR_WIDTH = 1280;
        saveFile << SCR_HEIGHT << endl;
        saveFile << SCR_WIDTH << endl;

        saveFile << currentPath << endl;

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

        for (int i = 0; i < 3; i++)
            saveFile >> lightAmbientColor[i];
        for (int i = 0; i < 3; i++)
            saveFile >> lightDiffuseColor[i];
        for (int i = 0; i < 3; i++)
            saveFile >> lightSpecularColor[i];

        saveFile >> lightLinear;
        saveFile >> lightQuatratic;

        saveFile >> SCR_HEIGHT;
        saveFile >> SCR_WIDTH;

        saveFile >> currentPath;

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

    lightAmbientColor[0] = 0.05f;
    lightAmbientColor[1] = 0.05f;
    lightAmbientColor[2] = 0.05f;
    lightDiffuseColor[0] = 0.8f;
    lightDiffuseColor[1] = 0.8f;
    lightDiffuseColor[2] = 0.8f;
    lightSpecularColor[0] = 1.0f;
    lightSpecularColor[1] = 1.0f;
    lightSpecularColor[2] = 1.0f;
    lightLinear = 0.09f;
    lightQuatratic = 0.032f;

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

    camera.RotateCamera(xoffset * RotateSensitivity, yoffset * RotateSensitivity);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.MoveCameraForward(static_cast<float>(yoffset) * ForwardSensitivity);
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