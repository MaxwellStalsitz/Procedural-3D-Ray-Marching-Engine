//Maxwell Stalsitz, 2022 

//stb_image library, for image loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//imgui libaries, for gui
#include "imconfig.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

//glad, glfw, and other fundamental library files
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
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

//source and shader header files, for setting up variables and shader class
#include "Source.h"
#include "Shader.h"

//screen width and height
unsigned int screenWidth = 1920;
unsigned int screenHeight = 1080;

//window variable
GLFWwindow* window;

//delta time variables
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//default height set for player
float terrainHeight = 0.0f;

//camera variables for orientation
glm::vec3 cameraPos = glm::vec3(0.0f, terrainHeight, 0.0f);
glm::vec3 desiredPos = cameraPos;
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//camera speed variable
float cameraSpeed = walkSpeed;

//mouse/camera variables
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 75.0f;

int WinMain()
{
    //boilerplate code, initializing glfw and setting the correct versions (modern opengl/core profile)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

    //creating the window with according width and height
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Rendering Engine", glfwGetPrimaryMonitor(), NULL);

    //fail case for glfw
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //boilerplate code, setting up glfw and the input functions
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    //fail case for GLAD library
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //setting application thumbnail
    GLFWimage images[1];
    images[0].pixels = stbi_load("images/enginethumbnail.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    //shader setup
    Shader rayMarchingShader("rmvertex.glsl", "rmfragment.glsl");
    Shader rayTracingShader("rtvertex.glsl", "rtfragment.glsl");

    //making sure loaded images are in the proper orientation (flipping them)
    stbi_set_flip_vertically_on_load(true);

	//boilerplate code, setting up the vertex buffer object, vertex array object, and element buffer object
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

	// imgui initialization and customizing the color scheme
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 24.0f, NULL); // normal font
    ImFont* font2 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 12.0f, NULL); // smaller font
    ImFont* font3 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 36.0f, NULL); // larger font
    ImFont* font4 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 6.0f, NULL); // smallest font

    // ------------------------------------------------------------------------

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f};

    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };

    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    colors[ImGuiCol_PlotLines] = ImVec4(1.f, 1.f, 1.f, 1.0f);

    // ------------------------------------------------------------------------

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowRounding = 0.0;
    style.TabRounding = 0.0;
    style.ScrollbarRounding = 0.0;
    style.TabBorderSize = 1.0;
    style.FramePadding = ImVec2(8.0, 3.0);

    style.ItemSpacing = ImVec2(15.0, 4.0);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    
    // ------------------------------------------------------------------------

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    double limitFPS = 1.0 / 120.0;

    //game loop
    while (!glfwWindowShouldClose(window))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //delta time setup
        float currentFrame = glfwGetTime();

        //fixed delta time (because deltatime is not constant, meaning that it's not good for physics)
        fixedDeltaTime += (currentFrame - lastFrame) / limitFPS;
        deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;

        while (fixedDeltaTime >= 1.0) {
            processInput(window);
            fixedDeltaTime--;
        }

        //milliseconds per frame
        float mspf = deltaTime * 1000;

        //prevents camera jerk at start
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        //window refresh
        glClearColor(0.2588f, 0.5294f, 0.9607f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //imgui refresh
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (paused)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        //getting mouse position
        glfwGetCursorPos(window, &xpo, &ypo);

        // ------------------------------------------------------------------------
        //setting up shader (ray marching or ray tracing) and sending data to uniform variables in fragment shader

        if (rayMarching) {
            //ray marching variables

            rayMarchingShader.use();
            rayMarchingShader.setVec2("resolution", glm::vec2(screenWidth * 1.2857, screenHeight));
            rayMarchingShader.setFloat("time", glfwGetTime());
            rayMarchingShader.setFloat("deltaTime", deltaTime);
            rayMarchingShader.setVec2("mousePosition", glm::vec2(xpo, ypo));

            rayMarchingShader.setVec3("cameraPos", cameraPos);
            rayMarchingShader.setVec3("direction", direction);
            rayMarchingShader.setVec3("cameraFront", cameraFront);
			
            rayMarchingShader.setBool("useLighting", useLighting);
            rayMarchingShader.setVec3("lightPosition", lightPosition);

            rayMarchingShader.setInt("MAX_STEPS", MAX_STEPS);
            rayMarchingShader.setFloat("MAX_DIST", MAX_DIST);
            rayMarchingShader.setFloat("MIN_DIST", MIN_DIST);

            rayMarchingShader.setBool("antiAliasing", antiAliasing);

            rayMarchingShader.setInt("scene", scene);

            rayMarchingShader.setBool("ambientOcclusion", ambientOcclusion);
            rayMarchingShader.setInt("samples", occlusionSamples);

            rayMarchingShader.setFloat("power", power);
            rayMarchingShader.setInt("iterations", iterations);
            rayMarchingShader.setBool("animate", animate);
            rayMarchingShader.setFloat("timeMultiplier", timeMultiplier);

            rayMarchingShader.setInt("numberOfObjects", numberOfEntities);

            rayMarchingShader.setBool("reflections", reflections);
            rayMarchingShader.setFloat("reflectionVisibility", reflectionVisibility);

            rayMarchingShader.setBool("fogEnabled", fogEnabled);
            rayMarchingShader.setFloat("fogVisibility", fogVisibility);
            rayMarchingShader.setFloat("falloff", falloff);

            //sending array of object values to shader for scene editor
            //there is probably a better solution to this

            const char* name;

            for (int i = 0; i < numberOfEntities; ++i)
            {
                std::stringstream ss;
                ss << "objectPositions[" << i << "]";
                std::string str = ss.str();
                name = str.c_str();

                rayMarchingShader.setVec3(name, sceneArray[i].position);
                ss.str("");

                ss << "objectScale[" << i << "]";
                str = ss.str();
                name = str.c_str();

                rayMarchingShader.setVec3(name, sceneArray[i].scale);
                ss.str("");

                ss << "primitives[" << i << "]";
                str = ss.str();
                name = str.c_str();

                rayMarchingShader.setInt(name, sceneArray[i].shape);
                ss.str("");

                ss << "objectColors[" << i << "]";
                str = ss.str();
                name = str.c_str();

                rayMarchingShader.setVec3(name, sceneArray[i].color);
                ss.str("");

                ss << "objectRotations[" << i << "]";
                str = ss.str();
                name = str.c_str();

                rayMarchingShader.setVec3(name, sceneArray[i].rotation);
                ss.str("");
            }
            
            rayMarchingShader.setInt("primitive", primitiveSelected);        
        }
        else {
            //ray tracing variables

            rayTracingShader.use();
            rayTracingShader.setVec2("resolution", glm::vec2(screenWidth, screenHeight));
            rayTracingShader.setVec2("mouse", glm::vec2(xpo, ypo));
            rayTracingShader.setFloat("time", glfwGetTime());

            rayTracingShader.setInt("scene", scene);

            rayTracingShader.setVec3("cameraPos", cameraPos);
            rayTracingShader.setVec3("direction", direction);
            rayTracingShader.setVec3("cameraFront", cameraFront);

            rayTracingShader.setBool("fogEnabled", fogEnabled);
            rayTracingShader.setFloat("fogVisibility", fogVisibility);
            rayTracingShader.setFloat("falloff", falloff);

            rayTracingShader.setBool("useLighting", useLighting);
            rayTracingShader.setVec3("lightPosition", lightPosition);

            rayTracingShader.setBool("reflections", reflections);
            rayTracingShader.setInt("reflectionCount", reflectionCount);
            rayTracingShader.setFloat("visibility", reflectionVisibility);

            rayTracingShader.setBool("antiAliasing", antiAliasing);
            rayTracingShader.setBool("ambientOcclusion", ambientOcclusion);
        }

        // ------------------------------------------------------------------------

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // ------------------------------------------------------------------------
        ImGuiWindowFlags window_flags_transparent = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGuiWindowFlags window_flags_adjustable = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGuiWindowFlags window_flags_parameters = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
        ImGuiWindowFlags window_flags_child = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGuiWindowFlags window_flags_editor = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
        ImGuiWindowFlags window_flags_entityEditor = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGuiWindowFlags window_flags_info = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;
        // ------------------------------------------------------------------------

        //getting framerate
        float frameRate = ImGui::GetIO().Framerate;

        ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.96, screenHeight * 0.95));
        ImGui::SetNextWindowSize(ImVec2(150, 50));

        //fps counter (bottom right corner)
        if (!paused) {
            ImGui::Begin("FPS", nullptr, window_flags_transparent);
            ImGui::PushFont(font1);
            ImGui::Text("%i", static_cast<int>(frameRate));
            ImGui::PopFont();
            ImGui::End();
        }

		//custom screen resolution
        if (selectedItem == 0) {
            screenWidth = 1280;
            screenHeight = 720;
        }
        if (selectedItem == 1) {
            screenWidth = 1920;
            screenHeight = 1080;
        }
        if (selectedItem == 2) {
            screenWidth = 2560;
            screenHeight = 1440;
        }
		
        // ------------------------------------------------------------------------

        float editorWidth = 400;
        float editorHeight = 500;

        if (start == false) {
            ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
            ImGui::SetNextWindowPos(ImVec2((screenWidth - editorWidth) / 2, (screenHeight - editorHeight) / 2));
        }

        start = true;

        editorWidth = 500;
		editorHeight = 500;

        ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
        ImGui::SetNextWindowPos(ImVec2((screenWidth - editorWidth) / 2, (screenHeight - editorHeight) / 2));

        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {

                if (ImGui::BeginMenu("Options")) {
                    static const char* items[]{ "1280x720", "1920x1080", "2560x1440" };
                    ImGui::Combo("Resolution", &selectedItem, items, IM_ARRAYSIZE(items));

                    ImGui::EndMenu();
                }

                if (ImGui::MenuItem("Shut Down"))
                    glfwSetWindowShouldClose(window, GL_TRUE);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {

                if (ImGui::BeginMenu("Camera Settings")) {
                    ImGui::SliderFloat("Movement Multiplier", &movementMultiplier, 0.0f, 5.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Scene")) {
                static const char* scenes[]{ "Demo Scene", "Mandelbulb", "Scene Editor", "Cornell Box" };
                ImGui::Combo("##foo", &currentScene, scenes, IM_ARRAYSIZE(scenes));
                scene = currentScene + 1;

                ImGui::EndMenu();
            }

            if (ImGui::Button("Info")) {
                inInfoMenu = true;
            }

            std::string sceneText;

            switch (scene) {
                case(1):
                    sceneText = " Demo Scene ";
                    break;
                case(2):
					sceneText = " Mandelbulb ";
					break;
                case(3):
					sceneText = " Scene Editor ";
					break;
                case(4):
					sceneText = " Cornell Box ";
					break;
            }

            auto sceneTextWidth = ImGui::CalcTextSize(sceneText.c_str()).x;

            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - sceneTextWidth) * 0.99f);

            ImGui::Text(sceneText.c_str());

            centerText("OpenGL Rendering Engine");

            ImGui::EndMainMenuBar();
        }

        if (paused) {
            ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.64285, screenHeight*0.027));
            ImGui::SetNextWindowSize(ImVec2(screenWidth * 0.358, screenHeight*0.974));

            if (ImGui::Begin("Parameters", nullptr, window_flags_parameters)) {

                if (xpo < screenWidth * 0.64285) {
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                }
                else {
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
                }
                
                if (ImGui::BeginTabBar("Parameter Tabs")) {
                    if (ImGui::BeginTabItem(" Ray Marching ")) {
						
                        rayMarching = true;
                        inEditor = false;

                        ImGui::Separator();
                        ImGui::Text("Settings");
                        ImGui::Separator();
                        ImGui::PushFont(font2);
                        ImGui::Text("");
                        ImGui::PopFont();

                        ImGui::BeginChild("Settings Child", ImVec2(screenWidth * 0.358 * 0.99, screenHeight / 2.5));

                        //formatting and setting up imgui inputs for key variables

                        ImGui::SliderInt(" Steps", &MAX_STEPS, 1, 1000);
                        ImGui::SliderFloat(" Max Distance", &MAX_DIST, 0, 100);
                        ImGui::SliderFloat(" Min Distance", &MIN_DIST, 0.001f, 0.25f);

                        if (MIN_DIST == 0) {
                            MIN_DIST = 0.001f;
                        }

                        ImGui::Text("");
                        commonParameters();

                        ImGui::Indent(64.0f);

                        if (scene == 2) {
                            ImGui::Indent(-64.0f);
                            ImGui::Text("");
                            ImGui::Separator();
                            ImGui::Text("Mandelbulb");
                            ImGui::Separator();
                            ImGui::PushFont(font2);
                            ImGui::Text("");
                            ImGui::PopFont();

                            ImGui::SliderFloat("Power", &power, 1, 10);
                            ImGui::SliderInt("Iterations", &iterations, 1, 20);
                            ImGui::Text("");
                            ImGui::Checkbox("Animate", &animate);
                            ImGui::Indent(32.0f);

                            if (animate) {
                                ImGui::DragFloat("Time Multiplier", &timeMultiplier, 0.001, 0, 1, NULL, ImGuiSliderFlags_AlwaysClamp);
                            }
                            ImGui::Indent(32.0f);
                        }
                        ImGui::Indent(-64.0f);

                        ImGui::EndChild();

                        ImGui::EndTabItem();
                        
                    }//

                    if (scene == 1) {
                        if (ImGui::BeginTabItem(" Ray Tracing ")) {
                            
                            rayMarching = false;

                            ImGui::Separator();
                            ImGui::Text("Settings");
                            ImGui::Separator();
                            ImGui::PushFont(font2);
                            ImGui::Text("");
                            ImGui::PopFont();

                            ImGui::BeginChild("Settings Child", ImVec2(screenWidth * 0.358 * 0.99, screenHeight / 2.5));

                            commonParameters();

                            ImGui::EndChild();
                            ImGui::EndTabItem();
                        }
                    }

                    if (scene == 3) {
                        if (ImGui::BeginTabItem(" Editor Settings ")){
                            inEditor = true;

                            ImGui::SetNextItemOpen(true);

                            ImGui::Separator();
                            ImGui::Text("Scene Hierarchy");
                            ImGui::Separator();

                            ImGui::BeginChild("Scene Heirarchy Child", ImVec2(screenWidth * 0.358 * 0.99, screenHeight / 2.75));

                            //rendering heirarchy system through imgui
                            ImGui::SetNextItemOpen(true);
                            if (ImGui::TreeNode("Entities:"))
                            {
                                ImGuiTreeNodeFlags node_flags = window_flags_parameters;

                                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                                
                                for (int i = 0; i < numberOfEntities; i++) {
                                    std::string nodeName = " " + sceneArray[i].name;
                                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, nodeName.c_str(), i);
                                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                        node_clicked = i;
                                }

                                ImGui::TreePop();
                            }
                            ImGui::EndChild();

                            ImGui::Text("");

                            ImGui::Separator();
                            ImGui::Text("Entity Editing");
                            ImGui::Separator();

                            ImGui::BeginChild("Entity Editing", ImVec2(screenWidth * 0.358 * 0.99, screenHeight / 8));

                            if (node_clicked != -1) {
                                glm::vec3 position = sceneArray[node_clicked].position;
                                ImGui::InputFloat3("Position", &position.x);
                                sceneArray[node_clicked].position = position;

                                glm::vec3 rotation = sceneArray[node_clicked].rotation;
                                ImGui::DragFloat3("Rotation", &rotation.x, 1.0f, 0.0f, 360.0f);
                                sceneArray[node_clicked].rotation = rotation;

                                glm::vec3 scale = sceneArray[node_clicked].scale;
                                ImGui::InputFloat3("Scale", &scale.x);
                                sceneArray[node_clicked].scale = scale;

                                if (ImGui::Button("Delete")) { //delete entity
                                    sceneArray = removeElement(sceneArray, node_clicked);
                                    numberOfEntities--;			
                                    node_clicked = -1;
                                }

                                ImGui::SameLine();

                                static const char* primitives[]{ "Sphere", "Box", "Torus", "Octahedron", "Round Box", "Box Frame" };
                                ImGui::SetNextItemWidth(screenHeight * 0.285);
                                int changedPrimitive;
                                ImGui::Combo("##foo", &changedPrimitive, primitives, IM_ARRAYSIZE(primitives));
                            }
                            else {
                                ImGui::Text("");
                                ImGui::Text("");

                                centerText("No Entity Selected");
                            }

                            ImGui::EndChild();

                            ImGui::Separator();
                            ImGui::Text("Add Entity");
                            ImGui::Separator();

                            static char entityName[128] = "";

                            centerText(" Entity Name ");

                            ImGui::PushItemWidth(screenWidth / 2.85);
                            ImGui::InputText(" ", entityName, IM_ARRAYSIZE(entityName));
                            ImGui::PopItemWidth();

                            ImGui::InputFloat3(" Entity Position", &editorPosition.x);
                            ImGui::InputFloat3(" Entity Rotation", &editorRotation.x);
                            ImGui::InputFloat3(" Entity Scale", &editorScale.x);

                            ImGui::PushFont(font4);
                            ImGui::Text("");
                            ImGui::PopFont();

                            std::string materialColorText = "Material Color";
                            auto materialTextWidth = ImGui::CalcTextSize(materialColorText.c_str()).x;

                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - materialTextWidth) * 0.075f);
                            ImGui::Text(materialColorText.c_str());

                            ImGui::SameLine();

                            ImGui::PushFont(font3); // larger font size
                            std::string addText = " Create Entity ";
                            auto textWidth = ImGui::CalcTextSize(addText.c_str()).x;
                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 1.25);

                            if (ImGui::Button(addText.c_str()) && !(entityName[0] == '\0')) {
                                sceneObject entity;
                                entity.name = entityName;
                                entity.position = editorPosition;
                                entity.rotation = editorRotation;
                                entity.scale = editorScale;
                                entity.shape = primitiveSelected;
                                entity.color = glm::vec3(color.x, color.y, color.z);

                                if (numberOfEntities < 25) {
                                    sceneArray[numberOfEntities] = entity;
                                    numberOfEntities++;
                                }

                                sceneEditor = false;
                            }

                            ImGui::PopFont();

                            // ------------------------------------------------------------------------
                            //drop down for selecting entity shape

                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 1.25);

                            static const char* primitives[]{ "Sphere", "Box", "Torus", "Octahedron", "Round Box", "Box Frame" };
                            ImGui::SetNextItemWidth(screenHeight * 0.285);
                            ImGui::Combo("##foo", &primitiveSelected, primitives, IM_ARRAYSIZE(primitives));

                            // ------------------------------------------------------------------------
                            //imgui color picker for material, code from imgui demo scene

                            //
                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - (screenWidth * 0.02605) ) * 0.15f);
                            ImGui::SetCursorPosY((ImGui::GetWindowHeight() - (screenHeight * 0.0463) ) * 0.945);
                            //

                            if (ImGui::ColorButton("Material Color", color, window_flags_editor, ImVec2((screenWidth * 0.02605), (screenHeight * 0.0463)))) {
                                ImGui::OpenPopup("Color Picker");
                                backup_color = color;
                            }

                            if (ImGui::BeginPopup("Color Picker"))
                            {
                                ImGui::Text("Material Color");
                                ImGui::Separator();
                                ImGui::ColorPicker4("##picker", (float*)&color, window_flags_editor | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                                ImGui::SameLine();

                                ImGui::EndPopup();
                            }

                            ImGui::EndTabItem();
                        }
                    }
					
                    ImGui::EndTabBar();
                }

                // ------------------------------------------------------------------------
                //creating a graph to display frame rate over time

                ImGui::Indent(64.0f);

                static float values[25] = {};
                static int values_offset = 0;
                static double refresh_time = 0.1;

                if (refresh_time == 0.0)
                    refresh_time = ImGui::GetTime();
				
                while (refresh_time < ImGui::GetTime()) {
                    values[values_offset] = frameRate;
                    values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
                    refresh_time += 1.0f / 60.0f;
                }

                float average = 0.0f;
				
                for (int n = 0; n < IM_ARRAYSIZE(values); n++)
                    average += values[n];
                average /= (float)IM_ARRAYSIZE(values);

                char overlay[32];
                sprintf_s(overlay, "Average: %f", average); //writing out fps average above graph

                ImGui::Indent(-64.0f);

                // ------------------------------------------------------------------------

                if (!inEditor) {
                ImGui::PushFont(font2); // smaller font size
                ImGui::Text("");
                ImGui::PopFont();

                ImGui::Separator();
                ImGui::Text("Performance");
                ImGui::Separator();

                //getting data for graph 2, and displaying both graphs

                static float values2[25] = {};
                static int values_offset2 = 0;
                static double refresh_time2 = 0.1;

                if (refresh_time2 == 0.0)
                    refresh_time2 = ImGui::GetTime();

                while (refresh_time2 < ImGui::GetTime()) {
                    values2[values_offset] = mspf;
                    values_offset2 = (values_offset2 + 1) % IM_ARRAYSIZE(values2);
                    refresh_time2 += 1.0f / 60.0f;
                }

                float average2 = 0.0f;

                for (int n = 0; n < IM_ARRAYSIZE(values2); n++)
                    average2 += values2[n];
                average2 /= (float)IM_ARRAYSIZE(values2);

                char overlay2[32];
                sprintf_s(overlay2, "Average: %f", average2);

                
                ImGui::BeginChild("Graphs", ImVec2(screenWidth * 0.35, screenHeight / 3), false, window_flags_child);

                centerText("Frame Rate (FPS)");
                ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 1000.0f, ImVec2(screenWidth * 0.35, 350.0f));

                centerText("Milliseconds Per Frame");
                ImGui::PlotLines("", values2, IM_ARRAYSIZE(values2), values_offset2, overlay2, 0.0f, 100.0f, ImVec2(screenWidth * 0.35, 330.0f));

                ImGui::EndChild();

                ImGui::PushFont(font2);
                ImGui::Text("");
                ImGui::PopFont();
                ImGui::Separator();

                std::string resetText = " Reset to Defaults ";
                auto resetTextWidth = ImGui::CalcTextSize(resetText.c_str()).x;
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - resetTextWidth) / 2);

                //reset to defaults button
                if (ImGui::Button(resetText.c_str())) {
                    MAX_STEPS = 325;
                    MAX_DIST = 500.0f;
                    MIN_DIST = 0.02;
                    antiAliasing = false;
                    useLighting = true;
                    ambientOcclusion = true;
                    occlusionSamples = 8;
                    power = 8;
                    iterations = 8;
                    animate = true;
                    timeMultiplier = 1.0f;
                    reflections = false;
                    reflectionVisibility = 0.5f;
                    reflectionCount = 2;
                    fogEnabled = true;
                    fogVisibility = 1.0f;
                    lightPosition = glm::vec3(0, 2, 0);
                }

                ImGui::Separator();
                }

                ImGui::PopStyleVar();
                ImGui::End();
            }

            float infoWidth = screenWidth * 0.5;
            float infoHeight = screenHeight * 0.5;

            ImGui::SetNextWindowPos(ImVec2((screenWidth - infoWidth) * 0.5, (screenHeight - infoHeight) * 0.5));
            ImGui::SetNextWindowSize(ImVec2(infoWidth, infoHeight));
        }

        //info menu (rendered last)
        if (inInfoMenu) {
            if (!ImGui::Begin("Info", &inInfoMenu, window_flags_info)) {
                ImGui::End();
            }
            else {
                ImGui::TextWrapped("This project is realtime OpenGL rendering engine that has support for ray marching and ray tracing rendering.");
                ImGui::Text("");

                ImGui::TextWrapped("The engine interface was created using Dear ImGui, along with the following libraries:");
                ImGui::BulletText("OpenGL");
				ImGui::BulletText("GLFW");
				ImGui::BulletText("GLM");
				ImGui::BulletText("stb_image");
                ImGui::Text("");

                ImGui::TextWrapped("The engine has support for the following features/lighting effects:");
				ImGui::BulletText("Ray marching and Ray tracing rendering");
				ImGui::BulletText("Ambient occlusion");
				ImGui::BulletText("Reflections");
				ImGui::BulletText("Anti-aliasing");
                ImGui::BulletText("Phong lighting Model");
				ImGui::BulletText("Fractal Rendering");
                ImGui::BulletText("Performance-measuring utilities");
                ImGui::BulletText("Ray marching custom scene editor");
                ImGui::Text("");

                ImGui::TextWrapped("The project is divided into different scenes, which can be changed in the scene tab, found in upper-left corner.");
                ImGui::Text("");

                ImGui::TextWrapped("For the Demo Scene & Cornell Box Scene, you can switch tabs in the parameter window to change between ray marching and ray tracing, both with according settings.");
                ImGui::Text("");

                ImGui::TextWrapped("When in the Mandelbulb scene, you are provided with additional settings for rendering the 3D fractal. For higher detail, lower the Min Distance value.");
                ImGui::Text("");

                ImGui::TextWrapped("In the Scene Editor scene, you are able to add and customize new 3D primitives in a raymarching scene, and there are two sections, one for creating a scene and the other for modifying ray marching parameters.");
                ImGui::Text("");

                ImGui::TextWrapped("Made by Max Stalsitz.");

                ImGui::End();
            }
        }

        if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        //rendering imgui frame
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //rendering/refreshing window
        glfwSwapInterval(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //imgui shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
	
    //buffer and array object deletion
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    //glfw shutdown
    glfwTerminate();
    return 0;
}

void commonParameters() {
    ImGui::Checkbox("Anti-Aliasing (SSAA 4X)", &antiAliasing);
    ImGui::Text("");

    ImGui::Checkbox("Lighting", &useLighting);
    ImGui::Indent(32.0f);

    if (useLighting) {

        if (scene != 4) {
            ImGui::InputFloat3("Light Position", &lightPosition.x);
        }

        ImGui::Checkbox("Ambient Occlusion", &ambientOcclusion);
        ImGui::Indent(32.0f);
        if (ambientOcclusion)
            ImGui::SliderInt("Samples", &occlusionSamples, 1, 50);
        ImGui::Unindent(32.0f);
        ImGui::Checkbox("Reflections", &reflections);
        ImGui::Indent(32.0f);

        if (reflections) {
            ImGui::SliderFloat("Visibility", &reflectionVisibility, 0.0f, 1.0f);

            if (!rayMarching) ImGui::SliderInt("Count", &reflectionCount, 2, 5);
        }

        ImGui::Unindent(64.0f);
    }
    else
        ImGui::Indent(-32.0f);

    ImGui::Checkbox("Fog", &fogEnabled);

    if (fogEnabled) {
        ImGui::Indent(32.0f);
        ImGui::SliderFloat("Fog Visibility", &fogVisibility, 0.0f, 1.0f);

        int intFalloff = (int)falloff;
        ImGui::SliderInt("Fog Falloff", &intFalloff, 10, 100);
        falloff = (float)intFalloff;

        ImGui::Unindent(32.0f);
    }
}

std::vector<sceneObject> removeElement(std::vector<sceneObject> arr, int elementIndex) {
    std::vector<sceneObject>::iterator it = arr.begin();
    std::advance(it, elementIndex);
    arr.erase(it);

    return arr;
}

void centerText(std::string text) {
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

//mouse button input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    
}

//mouse input
void mouse_callback(GLFWwindow* window, double xposInput, double yposInput)
{
    float xpos = static_cast<float>(xposInput);
    float ypos = static_cast<float>(yposInput);

    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset, yoffset;

    if (!paused) {
        xoffset = xpos - lastX;
        yoffset = lastY - ypos;
    }
    else {
        xoffset = 0;
        yoffset = 0;
    }

    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

//mouse scroll whell input
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    
}

//keyboard input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == 1) {
        paused = !paused;
    }
}

//input (called every frame)
void processInput(GLFWwindow* window) 
{
    if (!paused) {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            cameraSpeed = glm::mix(cameraSpeed, (runSpeed / 2) * movementMultiplier, runSmoothing / 10);
        }
        else {
            cameraSpeed = glm::mix(cameraSpeed, (walkSpeed / 2) * movementMultiplier, runSmoothing / 10);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            desiredPos += (cameraSpeed)*glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            desiredPos -= (cameraSpeed)*glm::normalize(glm::vec3(direction.x, 0.0f, direction.z));
        }

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            desiredPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            desiredPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (cameraSpeed);
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            if (!jump) {
                jump = true;
                velocityY += jumpHeight;
            }
        }

        //camera smoothing
        cameraPos.x = glm::mix(cameraPos.x, desiredPos.x, smoothing / 100);
        cameraPos.z = glm::mix(cameraPos.z, desiredPos.z, smoothing / 100);

        cameraPos.y += velocityY * 0.01;

        if (cameraPos.y > terrainHeight * 0.02)
            velocityY += gravity * 0.01;
        else {
            jump = false;
            velocityY = 0.0f;
        }
    }
}

//window refresh
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}