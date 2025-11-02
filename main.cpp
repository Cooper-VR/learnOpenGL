#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <shaders/shader.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <camera/camera.hpp>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <assimp/Importer.hpp>      // for Assimp::Importer
#include <assimp/scene.h>           // for aiScene
#include <assimp/postprocess.h>     // for post-processing flags
#include <loaders/stb_image.h>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const *path);
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
float lightAmbientColor[4][3] = {{0.05f, 0.05f, 0.05f}, {0.05f, 0.05f, 0.05f}, {0.05f, 0.05f, 0.05f}, {0.05f, 0.05f, 0.05f}};
float lightDiffuseColor[4][3] = {{0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}, {0.8f, 0.8f, 0.8f}};
float lightSpecularColor[4][3] = {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}};
float lightLinear[4] = {0.09f, 0.09f, 0.09f, 0.09f};
float lightQuatratic[4] = {0.032f, 0.032f, 0.032f, 0.032f};

string pointLightAttribs[7] = {
    ".position",
    ".ambient",
    ".diffuse",
    ".specular",
    ".constant",
    ".linear",
    ".quadratic"};

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

    Shader lightingShader("resources/shaders/objectLighting_vertex.glsl", "resources/shaders/objectLighting_fragment.glsl");
    Shader lightCubeShader("resources/shaders/litObject_vertex.glsl", "resources/shaders/litObject_fragment.glsl");

    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f};

    vector<glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
    unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");
    lightingShader.use();
    lightingShader.setInt("matrial.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Start new ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("OpenGL UI");

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
                cubePositions.push_back(newPos);
            }
            mousePressLeft = true;
        }
        else
        {
            mousePressLeft = false;
        }

        glClearColor(skyColor[0], skyColor[1], skyColor[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model;

        

        for (int i = 0; i < 4; i++)
        {
            ImGui::PushID(i);
            ImGui::Text("light: %d", i);
            ImGui::ColorEdit3("LightAmbientColor", lightAmbientColor[i]);
            ImGui::ColorEdit3("LightDiffuseColor", lightDiffuseColor[i]);
            ImGui::ColorEdit3("LightSpecularColor", lightSpecularColor[i]);
            ImGui::InputFloat("light linear", &lightLinear[i], 0.01, 0.01);
            ImGui::InputFloat("light quadratic", &lightQuatratic[i], 0.005, 0.05);
            ImGui::Separator();
            ImGui::PopID();

            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));

            lightCubeShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            lightCubeShader.setMat4("model", model);
            lightCubeShader.setVec3("mainColor", lightDiffuseColor[i][0], lightDiffuseColor[i][1], lightDiffuseColor[i][2]);

            glBindVertexArray(lightVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if (ImGui::Button("Save Data"))
        {
            saveData();
        }
        if (ImGui::Button("reset save data"))
        {
            resetData();
        }

        ImGui::End();

        lightingShader.use();
        // material properties
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        lightingShader.setVec3("ViewDir", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", dirLightAmbientColor[0], dirLightAmbientColor[1], dirLightAmbientColor[2]);
        lightingShader.setVec3("dirLight.diffuse", dirLightDiffuseColor[0], dirLightDiffuseColor[1], dirLightDiffuseColor[2]);
        lightingShader.setVec3("dirLight.specular", dirLightSpecularColor[0], dirLightSpecularColor[1], dirLightSpecularColor[2]);

        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        for (int i = 0; i < 4; i++)
        {
            lightingShader.setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[0], pointLightPositions[i]);
            lightingShader.setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[1], lightAmbientColor[i][0], lightAmbientColor[i][1], lightAmbientColor[i][2]);
            lightingShader.setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[2], lightDiffuseColor[i][0], lightDiffuseColor[i][1], lightDiffuseColor[i][2]);
            lightingShader.setVec3("pointLights[" + to_string(i) + "]" + pointLightAttribs[3], lightSpecularColor[i][0], lightSpecularColor[i][1], lightSpecularColor[i][2]);
            lightingShader.setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[4], 1.0f);
            lightingShader.setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[5], lightLinear[i]);
            lightingShader.setFloat("pointLights[" + to_string(i) + "]" + pointLightAttribs[6], lightQuatratic[i]);
        }

        for (unsigned int i = 0; i < cubePositions.size(); i++)
        {
            model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, diffuseMap);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, specularMap);

            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup

    cout << "closing application" << endl;

    // get the imguiu stuff store it in a file
    saveData();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
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
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 3; i++)
                saveFile << lightAmbientColor[j][i] << ' ';
            saveFile << endl;
            for (int i = 0; i < 3; i++)
                saveFile << lightDiffuseColor[j][i] << ' ';
            saveFile << endl;
            for (int i = 0; i < 3; i++)
                saveFile << lightSpecularColor[j][i] << ' ';

            saveFile << endl;
            saveFile << lightLinear[j] << endl;
            saveFile << lightQuatratic[j] << endl;
        }

        saveFile << SCR_HEIGHT << endl;
        saveFile << SCR_WIDTH << endl;

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
        for (int j = 0; j < 4; j++)
        {
            for (int i = 0; i < 3; i++)
                saveFile >> lightAmbientColor[j][i];
            for (int i = 0; i < 3; i++)
                saveFile >> lightDiffuseColor[j][i];
            for (int i = 0; i < 3; i++)
                saveFile >> lightSpecularColor[j][i];

            saveFile >> lightLinear[j];
            saveFile >> lightQuatratic[j];
        }

        saveFile >> SCR_HEIGHT;
        saveFile >> SCR_WIDTH;

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

    //reset all light colors
    for (int i = 0; i < 4; i++)
    {
        lightAmbientColor[i][0] = 0.05f;
        lightAmbientColor[i][1] = 0.05f;
        lightAmbientColor[i][2] = 0.05f;
        lightDiffuseColor[i][0] = 0.8f;
        lightDiffuseColor[i][1] = 0.8f;
        lightDiffuseColor[i][2] = 0.8f;
        lightSpecularColor[i][0] = 1.0f;
        lightSpecularColor[i][1] = 1.0f;
        lightSpecularColor[i][2] = 1.0f;
        lightLinear[i] = 0.09f;
        lightQuatratic[i] = 0.032f;
    }
}

void processInput(GLFWwindow *window)
{
    
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        //nothing
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
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS){
            //std::cout << "Middle mouse held!\n";
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
        //camera rotation
        if (!mousePressRight) xoffset = yoffset = 0.0f;
        mousePressRight = true;
        
    }else{
        mousePressRight = false;
    }

    

    camera.RotateCamera(xoffset * RotateSensitivity, yoffset * RotateSensitivity);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.MoveCameraForward(static_cast<float>(yoffset) * ForwardSensitivity);
}

unsigned int loadTexture(const char *path){
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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
    }

    return textureID;
}