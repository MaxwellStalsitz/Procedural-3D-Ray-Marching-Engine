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
    //initializing glfw and setting the correct versions (modern opengl/core profile)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

    //creating the window with according width and height
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Science Project", glfwGetPrimaryMonitor(), NULL);

    //fail case for glfw
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //setting up glfw and the input functions
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

    //shader setup
    Shader rayMarchingShader("rmvertex.glsl", "rmfragment.glsl");
    Shader frameBufferShader("fbvertex.glsl", "fbfragment.glsl");

    //setting the windows icon
    GLFWimage images[1];
    images[0].pixels = stbi_load("textures/moss.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    //making sure loaded images are in the proper orientation (flipping them)
    stbi_set_flip_vertically_on_load(true);

	//setting up the vertex buffer object, vertex array object, and element buffer object
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
    ImFont* font1 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 24.0f, NULL);
    ImFont* font2 = io.Fonts->AddFontFromFileTTF("fonts/mainfont.ttf", 12.0f, NULL);

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
	
    //framebuffer, for rendering to imgui

    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint depthrenderbuffer;
    glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, DrawBuffers);

    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };

    double limitFPS = 1.0 / 120.0;

    //game loop
    while (!glfwWindowShouldClose(window))
    {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        //delta time setup
        float currentFrame = glfwGetTime();

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
        
        //input
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glViewport(0, 0, screenWidth, screenHeight);

        //window refresh
        glClearColor(0.2588f, 0.5294f, 0.9607f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
        //setting up rayMarchingShader and sending data to uniform variables
        rayMarchingShader.use();
        rayMarchingShader.setVec2("resolution", glm::vec2(screenWidth * 1.2857, screenHeight));
        rayMarchingShader.setFloat("time", glfwGetTime());
        rayMarchingShader.setFloat("deltaTime", deltaTime);
        rayMarchingShader.setVec2("mousePosition", glm::vec2(xpo, ypo));
		
        rayMarchingShader.setVec3("cameraPos", cameraPos);
        rayMarchingShader.setVec3("direction", direction);
        rayMarchingShader.setVec3("cameraFront", cameraFront);
        rayMarchingShader.setBool("useLighting", useLighting);
		
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
        // ------------------------------------------------------------------------
        
        const char* name;

        for (int i = 0; i < numberOfEntities; ++i)
        {
            std::stringstream ss;
            ss << "objectPositions[" << i << "]";
            std::string str = ss.str();
			name = str.c_str();

            rayMarchingShader.setVec3(name, sceneArray[i].position);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //setting up framebuffer, and binding it to a texture to be rendered in the imgui window (also allows for post-processing effects)
        frameBufferShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, renderedTexture);

        // ------------------------------------------------------------------------
        ImGuiWindowFlags window_flags_transparent = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;
        ImGuiWindowFlags window_flags_adjustable = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing;
        ImGuiWindowFlags window_flags_parameters = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGuiWindowFlags window_flags_child = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
		ImGuiWindowFlags window_flags_editor = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		ImGuiWindowFlags window_flags_entityEditor = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        // ------------------------------------------------------------------------

        //getting framerate
        float frameRate = ImGui::GetIO().Framerate;

        ImGui::SetNextWindowPos(ImVec2(screenWidth*0.96, screenHeight*0.95));
        ImGui::SetNextWindowSize(ImVec2(150, 50));

        if (!paused) {
            ImGui::Begin("FPS", nullptr, window_flags_transparent);
            ImGui::PushFont(font1);
            ImGui::Text("%i", static_cast<int>(frameRate));
            ImGui::PopFont();
            ImGui::End();
        }

        if (selectedItem == 0) {
            screenWidth = 1280;
            screenHeight = 720;
        }
        else if (selectedItem == 1) {
            screenWidth = 1920;
            screenHeight = 1080;
        }
        else if (selectedItem == 2) {
            screenWidth = 2560;
            screenHeight = 1440;
        }

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

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight));

        if (ImGui::Begin("Scene", nullptr, window_flags_adjustable)) {
            ImGui::Image((ImTextureID)renderedTexture, ImVec2(screenWidth, screenHeight), ImVec2(0, 1), ImVec2(1, 0));

            ImGui::End();
        }

        if (ImGui::BeginMainMenuBar()){
            if (ImGui::BeginMenu("File")){

                if(ImGui::BeginMenu("Options")) {
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
                static const char* scenes[]{ "Demo Scene", "Mandelbulb", "Scene Editor" };
                ImGui::Combo("##foo", &currentScene, scenes, IM_ARRAYSIZE(scenes));
                scene = currentScene + 1;

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (paused) {
            ImGui::SetNextWindowPos(ImVec2(screenWidth * 0.64285, 0));
            ImGui::SetNextWindowSize(ImVec2(screenWidth * 0.358, screenHeight));

            if (ImGui::Begin("Parameters", nullptr, window_flags_parameters)) {

                if (xpo < screenWidth * 0.64285) {
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                }
                else {
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
                }
                
                if (ImGui::BeginTabBar("Parameter Tabs")) {
                    if (ImGui::BeginTabItem(" Ray Marching ")) {
                        inEditor = false;

                        ImGui::Separator();
                        ImGui::Text("Settings");
                        ImGui::Separator();
                        ImGui::Text("");

                        //formatting and setting up imgui inputs for key variables

                        ImGui::SliderInt("Max Steps", &MAX_STEPS, 1, 1000);
                        ImGui::SliderFloat("Max Distance", &MAX_DIST, 0, 2500);
                        ImGui::SliderFloat("Min Distance", &MIN_DIST, 0, 5);
                        ImGui::Text("");
                        ImGui::Checkbox("Anti-Aliasing (SSAA 4X)", &antiAliasing);
                        ImGui::Text("");
                        ImGui::Checkbox("Lighting", &useLighting);
                        ImGui::Indent(32.0f);
                        if (useLighting) {
                            ImGui::Checkbox("Ambient Occlusion", &ambientOcclusion);
                            ImGui::Indent(32.0f);
                            if (ambientOcclusion)
                                ImGui::SliderInt("Samples", &occlusionSamples, 1, 50);
							ImGui::Unindent(64.0f);
                            ImGui::Checkbox("Reflections", &reflections);
                            ImGui::Indent(64.0f);
                        }
                        else
                            ImGui::Indent(32.0f);

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
                            if (animate) {
								ImGui::Indent(32.0f);
								ImGui::SliderFloat("Time Multiplier", &timeMultiplier, 0, 1);
                            }
                            ImGui::Indent(32.0f);
                        }
                        ImGui::Indent(-64.0f);
						
                        ImGui::EndTabItem();
                    }

                    if (scene == 1) {
                        if (ImGui::BeginTabItem(" Rasterization ")) {


                            ImGui::EndTabItem();
                        }
                    }

                    //int node_clicked;

                    if (scene == 3) {
                        if (ImGui::BeginTabItem(" Editor Settings ")){
                            inEditor = true;

                            ImGui::Text("");

                            ImGui::SetNextItemOpen(true);

                            if (ImGui::TreeNode("Scene Heirarchy"))
                            {
                                ImGuiTreeNodeFlags node_flags = window_flags_parameters;

                                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet
                                
                                for (int i = 0; i < numberOfEntities; i++) {
                                    std::string nodeName = sceneArray[i].name;
                                    ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, nodeName.c_str(), i);
                                    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                                        node_clicked = i;
                                }

                                ImGui::TreePop();
                            }

                            if (node_clicked != -1) {
                                ImGui::Text("");
                                glm::vec3 position = sceneArray[node_clicked].position;
                                ImGui::InputFloat3("Position", &position.x);
                                sceneArray[node_clicked].position = position;
                            }

                            ImGui::Text("");
                            ImGui::Separator();
                            ImGui::Text("Add Entity");
                            ImGui::Separator();
                            ImGui::Text("");

                            static char entityName[128] = "";

                            std::string nameText = " Entity Name ";
                            auto nameTextWidth = ImGui::CalcTextSize(nameText.c_str()).x;
                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - nameTextWidth) / 2);
                            ImGui::Text(nameText.c_str());

                            std::string text2 = " ";
                            auto textWidth2 = ImGui::CalcTextSize(text2.c_str()).x;
                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth2) / 2);
                            ImGui::InputText(text2.c_str(), entityName, IM_ARRAYSIZE(entityName));

                            ImGui::InputFloat3("Position", &editorPosition.x);
                            ImGui::InputFloat3("Rotation", &editorRotation.x);
                            ImGui::InputFloat3("Scale", &editorScale.x);

                            // ------------------------------------------------------------------------
                            //imgui color picker for material
                            static ImVec4 color = ImVec4(114.0f / 255.0f, 144.0f / 255.0f, 154.0f / 255.0f, 200.0f / 255.0f);
                            static ImVec4 backup_color;

                            static bool saved_palette_init = true;
                            static ImVec4 saved_palette[32] = {};
                            if (saved_palette_init) {
                                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++) {
                                    ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                                        saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                                    saved_palette[n].w = 1.0f; // Alpha
                                }
                                saved_palette_init = false;
                            }

                            if (ImGui::ColorButton("MyColor##3b", color, window_flags_editor)) {
                                ImGui::OpenPopup("mypicker");
                                backup_color = color;
                            }

                            ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  
                            if (ImGui::BeginPopup("mypicker"))
                            {
                                ImGui::Text("Material Color");
                                ImGui::Separator();
                                ImGui::ColorPicker4("##picker", (float*)&color, window_flags_editor | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                                ImGui::SameLine();

                                ImGui::BeginGroup();
                                ImGui::Text("Current");
                                ImGui::ColorButton("##current", color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
                                ImGui::Text("Previous");
                                if (ImGui::ColorButton("##previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                                    color = backup_color;
                                ImGui::Separator();
                                ImGui::Text("Palette");
                                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
                                {
                                    ImGui::PushID(n);
                                    if ((n % 8) != 0)
                                        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                                    ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                                    if (ImGui::ColorButton("##palette", saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                                        color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w);

                                    if (ImGui::BeginDragDropTarget())
                                    {
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                                        ImGui::EndDragDropTarget();
                                    }

                                    ImGui::PopID();
                                }
                                ImGui::EndGroup();
                                ImGui::EndPopup();
                            }
                            // ------------------------------------------------------------------------

                            ImGui::Text("Material Color");
                            ImGui::Text("");

                            std::string addText = "Add";
                            auto textWidth = ImGui::CalcTextSize(addText.c_str()).x;
                            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 2);

                            if (ImGui::Button("  Add  ")) {
                                if (!(entityName[0] == '\0')) {
                                    sceneObject entity;
                                    entity.name = entityName;
                                    entity.position = editorPosition;
                                    entity.rotation = editorRotation;
                                    entity.scale = editorScale;

                                    if (numberOfEntities < 25) {
                                        sceneArray[numberOfEntities] = entity;
                                        numberOfEntities++;
                                    }

                                    sceneEditor = false;
                                }
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
                sprintf_s(overlay, "Average: %f", average);

                ImGui::Indent(-64.0f);

                if (scene != 3) {
                    ImGui::PushFont(font2);
                    ImGui::Text("");
                    ImGui::PopFont();
                    ImGui::Separator();
                    ImGui::Text("Graphs");
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

                    if (ImGui::BeginChild("Graphs", ImVec2(screenWidth * 0.35, screenHeight / 3), false, window_flags_child)) {
                        std::string text = "Frame Rate (FPS)";
                        auto textWidth = ImGui::CalcTextSize(text.c_str()).x;
                        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 2);
                        ImGui::Text(text.c_str());

                        ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, 0.0f, 1000.0f, ImVec2(screenWidth * 0.35, 350.0f));

                        std::string mspfText = "Milliseconds Per Frame";
                        auto mspfTextWidth = ImGui::CalcTextSize(mspfText.c_str()).x;
                        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - mspfTextWidth) / 2);
                        ImGui::Text(mspfText.c_str());
                        ImGui::PlotLines("", values2, IM_ARRAYSIZE(values2), values_offset2, overlay2, 0.0f, 100.0f, ImVec2(screenWidth * 0.35, 330.0f));

                        ImGui::EndChild();
                    }
                }
                // ------------------------------------------------------------------------


                ImGui::PushFont(font2);
                ImGui::Text("");
                ImGui::PopFont();
                ImGui::Separator();

                if (!inEditor) {

                    std::string resetText = " Reset to Defaults ";
                    auto resetTextWidth = ImGui::CalcTextSize(resetText.c_str()).x;
                    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - resetTextWidth) / 2);

                    //reset to defaults button
                    if (ImGui::Button(resetText.c_str())) {
                        MAX_STEPS = 256;
                        MAX_DIST = 500.0f;
                        MIN_DIST = 0.02;
                        antiAliasing = false;
                        useLighting = true;
                        ambientOcclusion = true;
                        occlusionSamples = 8;
                        power = 7;
                        iterations = 8;
                        animate = true;
                        timeMultiplier = 1.0f;
                    }
                    ImGui::Separator();
                }

                ImGui::PopStyleVar();
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