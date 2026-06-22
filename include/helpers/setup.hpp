#ifndef setup_hpp
#define setup_hpp

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <camera/camera.hpp>
#include <helpers/sceneTree.hpp>
#include <helpers/octree.hpp>

extern Camera camera;

extern float RotateSensitivity;
extern float PanSensitivity;
extern float ForwardSensitivity;

extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float lastX;
extern float lastY;
extern bool firstMouse;

extern bool mousePressLeft;
extern bool mousePressRight;

extern float deltaTime;
extern float lastFrame;

extern unsigned int frameID;

using namespace std;

extern unsigned int rbo;
extern float quadVertices[];

extern unsigned int quadVAO, quadVBO;
extern unsigned int framebuffer;
extern unsigned int textureColorbuffer;
extern GLuint depthTexture;
extern Shader *screenShader;

void processInput(GLFWwindow *window);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

GLFWwindow *setupOpenGL();

void setUpImGui(GLFWwindow *window);

vector<OctreeNode*> getCulledOctreeNodes(OctreeNode *root, Frustum cameraFrustum);

void getChildrenCulledOctreeNodes(OctreeNode *node, Frustum cameraFrustum, vector<OctreeNode*> &culledNodes);

#endif