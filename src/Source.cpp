//Maxwell Stalsitz, 2022

//stb_image library, for image loading
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

//source and shader header files, for setting up variables and shader class
#include "Source.h"
#include "Shader.h"
#include "cameraMovement.h"
#include "shaderSetup.h"
#include "input.h"
#include "maingui.h"
#include "sceneEditor.h"

//window variable
GLFWwindow* window;

Shader * rayMarchingShader;
Shader * rayTracingShader;

unsigned int VBO, VAO, EBO;

double lastFrameTime;
double limitFPS;

int init(){
    //boilerplate code, initializing glfw and setting the correct versions (modern opengl/core profile)
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

    //creating the window with according width and height
    window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Rendering Engine", glfwGetPrimaryMonitor(), nullptr);

    //fail case for glfw
    if (window == nullptr){
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
    images[0].pixels = stbi_load("../Resources/images/enginethumbnail.png", &images[0].width, &images[0].height, nullptr, 4); //rgba channels
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    //shader setup
    rayMarchingShader = new Shader("../src/shaders/rmvertex.glsl", "../src/shaders/rmfragment.glsl");
    rayTracingShader = new Shader("../src/shaders/rtvertex.glsl", "../src/shaders/rtfragment.glsl");

    //making sure loaded images are in the proper orientation (flipping them)
    stbi_set_flip_vertically_on_load(true);


    //boilerplate code, setting up the vertex buffer object, vertex array object, and element buffer object
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    styleInitialization();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    return 1;
}

void update(){
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    //delta time setup
    float currentFrame = (float)glfwGetTime();

    //fixed delta time (because delta time is not constant)
    fixedDeltaTime += (currentFrame - lastFrame) / limitFPS;
    deltaTime = currentFrame - lastFrame;

    lastFrame = currentFrame;

    while (fixedDeltaTime >= 1.0) {
        processInput(window);

        //camera smoothing
        cameraPos.x = glm::mix(cameraPos.x, desiredPos.x, smoothing / 100);
        cameraPos.z = glm::mix(cameraPos.z, desiredPos.z, smoothing / 100);

        fixedDeltaTime--;
    }

    //milliseconds per frame
    float mspf = deltaTime * 1000;

    //prevents camera jerk at start
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    //window refresh
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
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
        rayMarchingShader->use();
        setupRayMarching(rayMarchingShader);
    }
    else {
        rayTracingShader->use();
        setupRayTracing(rayTracingShader);
    }

    // ------------------------------------------------------------------------
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    fpsCounter();

    //custom screen resolution
    switch(selectedItem){
        case(0):
            screenWidth = 1280;
            screenHeight = 720;

            ImGui::GetIO().FontGlobalScale = 0.666f;
            break;
        case(1):
            screenWidth = 1920;
            screenHeight = 1080;

            ImGui::GetIO().FontGlobalScale = 1.0f;
            break;
        case(2):
            screenWidth = 2560;
            screenHeight = 1440;

            ImGui::GetIO().FontGlobalScale = 1.333f;
            break;
    }

    glfwSetWindowSize(window, screenWidth, screenHeight);

    //some devices automatically set a certain resolution upon change, so this call updates the screenWidth & screenHeight values
    glfwGetWindowSize(window, &screenWidth, &screenHeight);

    // ------------------------------------------------------------------------

    float editorWidth = 0.2083f * (float)screenWidth;
    float editorHeight = 0.26042f * (float)screenHeight;

    if (!start) {
        ImGui::SetNextWindowSize(ImVec2(editorWidth, editorHeight));
        ImGui::SetNextWindowPos(ImVec2((screenWidth - editorWidth) / 2, (screenHeight - editorHeight) / 2));
    }

    start = true;

    editorWidth = 500;
    editorHeight = 500;

    ImGui::SetNextWindowSize(ImVec2((float)editorWidth, (float)editorHeight));
    ImGui::SetNextWindowPos(ImVec2(((float)screenWidth - (float)editorWidth) / 2, (screenHeight - editorHeight) / 2));

    menuBar(window);

    //info menu (rendered last)
    if (inInfoMenu) {
        float infoWidth = (float)screenWidth * 0.5f;
        float infoHeight = (float)screenHeight * 0.5f;

        ImGui::SetNextWindowPos(ImVec2((screenWidth - infoWidth) * 0.5f, (screenHeight - infoHeight) * 0.5f));
        ImGui::SetNextWindowSize(ImVec2(infoWidth, infoHeight));

        ImGui::Begin("Info", &inInfoMenu, window_flags_info);
        infoMenuText();
        ImGui::End();
    }

    if (paused) {

        ImGuiStyle& style = ImGui::GetStyle();
        float titleBarHeight = (style.WindowPadding.y - 3) + ImGui::GetFontSize();

        ImGui::SetNextWindowPos(ImVec2((float)screenWidth*0.64285f, titleBarHeight));
        ImGui::SetNextWindowSize(ImVec2((float)screenWidth*0.358f, (float)screenHeight + titleBarHeight));

        if (ImGui::Begin("Parameters", nullptr, window_flags_parameters)) {

            if (xpo < (screenWidth * 0.64285f)) {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
            }
            else {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.f);
            }

            if (ImGui::BeginTabBar("Parameter Tabs")) {

                if (ImGui::BeginTabItem(" Ray Marching ")) {
                    rayMarchingScene();
                    ImGui::EndTabItem();
                }

                if ((scene == 1 || scene == 6) && ImGui::BeginTabItem(" Ray Tracing ")) {
                    rayTracingScene();
                    ImGui::EndTabItem();
                }

                if (scene == 3 && ImGui::BeginTabItem(" Editor Settings ")){
                    sceneEditorGui();
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            // ------------------------------------------------------------------------
            //creating a graph to display frame rate over time, along with options for capping fps

            performanceGraph(ImGui::GetIO().Framerate, mspf);

            // ------------------------------------------------------------------------

            ImGui::PushFont(font2);
            ImGui::Text("");
            ImGui::PopFont();
            ImGui::Separator();

            std::string resetText = " Reset to Defaults ";
            auto resetTextWidth = ImGui::CalcTextSize(resetText.c_str()).x;
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - resetTextWidth) / 2);

            //reset to defaults button
            if (ImGui::Button(resetText.c_str())) {
                defaultsButton();
            }

            ImGui::Separator();

            ImGui::PopStyleVar();
            ImGui::End();
        }


    }

    if (ImGui::GetMouseCursor() == ImGuiMouseCursor_ResizeNWSE)
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

    //rendering imgui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (fpsCap) {
        frameCap(lastFrameTime);
        lastFrameTime += 1.0/frameLimit;
    }
    else
        lastFrameTime = glfwGetTime();

    //rendering/refreshing window
    glfwSwapInterval(0);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int WinMain()
{
    if(init() != 1)
        return -1;

    limitFPS;
    limitFPS = 1.0 / frameLimit;

    lastFrameTime = glfwGetTime();

    //game loop
    while (!glfwWindowShouldClose(window)){
        update();
    }

    //imgui shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //buffer and array object deletion
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    delete rayMarchingShader;
    delete rayTracingShader;

    //glfw shutdown
    glfwTerminate();
    return 0;
}

void frameCap(double lastFrameTime){
    while (glfwGetTime() < (lastFrameTime + 1.0/frameLimit)) {
        //there is nothing here because the function acts as a simple delay and waits
    }
}

std::vector<sceneObject> removeElement(std::vector<sceneObject> arr, int elementIndex) {
    auto it = arr.begin();
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

void defaultsButton(){
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
    fogEnabled = true;
    fogVisibility = 1.0f;
    lightPosition = glm::vec3(0, 2, 0);
    frameLimit = 120;
}
