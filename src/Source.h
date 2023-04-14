#ifndef SOURCE_H
#define SOURCE_H

//imgui libaries, for gui
#include "imconfig.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//glad, glfw, and other fundamental library files
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <vector>

//glm libraries, for complex math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

void processInput(GLFWwindow* window);
void centerText(std::string text);
void defaultsButton();

unsigned int texture;

double xpo, ypo;

glm::vec3 direction;

float fixedDeltaTime;

bool paused = false;
bool inEditor = false;
static bool inInfoMenu = false;

//screen width and height
int screenWidth = 1920;
int screenHeight = 1080;

bool fpsCap = true;
int frameLimit = 120;
void frameCap(double lastFrameTime);

// ------------------------------------------------------------------------
void commonParameters();

bool rayMarching = true;

bool useLighting = true;

int MAX_STEPS = 325;
float MAX_DIST = 100.0;
float MIN_DIST = 0.02;

bool antiAliasing = false;

bool ambientOcclusion = true;
int occlusionSamples = 8;

float power = 8;
int iterations = 8;

bool animate = true;
float timeMultiplier = 1.0f;

bool reflections = false;
float reflectionVisibility = 0.5f;

bool fogEnabled = true;
float fogVisibility = 1.0f;
float falloff = 50.0f;

float smoothness = 0.5f;

glm::vec3 lightPosition = glm::vec3(0,2,0);
// ------------------------------------------------------------------------
bool start = false;

bool sceneEditor = false;
int selectedItem = 1;

int currentScene = 0;
int scene = 1;

// ------------------------------------------------------------------------
int numberOfEntities = 0;
int node_clicked = -1;

static ImVec4 color = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static ImVec4 backup_color;

int primitiveSelected = 0;
int changedPrimitive;

glm::vec3 editorPosition = glm::vec3(0.0f);
glm::vec3 editorRotation = glm::vec3(0.0f);
glm::vec3 editorScale = glm::vec3(1.0f);

//sceneObject class, for rendering custmo objects on the fly
class sceneObject {
    public:
        std::string name;
        int shape;
        glm::vec3 position;
        glm::vec3 scale;
        glm::vec3 rotation;
        glm::vec3 color;
};

std::vector<sceneObject> sceneArray(25);

std::vector<sceneObject> removeElement(std::vector<sceneObject> arr, int elementIndex);
// ------------------------------------------------------------------------

float vertices[] = {
     1.f,  1.f, 0.0f,   1.0f, 1.0f,
     1.f, -1.f, 0.0f,   1.0f, 0.0f,
    -1., -1.f, 0.0f,   0.0f, 0.0f,
    -1.,  1.f, 0.0f,   0.0f, 1.0f
};

unsigned int indices[] = {
    0, 1, 3, 
    1, 2, 3 
};

#endif
