#ifndef MAINGUI_H
#define MAINGUI_H

#include "Source.h"

ImFont* font1;
ImFont* font2;
ImFont* font3;
ImFont* font4;

ImGuiWindowFlags window_flags_transparent = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus;
ImGuiWindowFlags window_flags_parameters = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNavFocus;
ImGuiWindowFlags window_flags_child = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus;
ImGuiWindowFlags window_flags_editor = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
ImGuiWindowFlags window_flags_info = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize;

void styleInitialization(){

    // imgui initialization and customizing the color scheme
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    font1 = io.Fonts->AddFontFromFileTTF("C:/Users/maxst/Downloads/Procedural-3D-Ray-Marching-Engine/Resources/fonts/mainfont.ttf", 24.0f, nullptr); // normal font
    font2 = io.Fonts->AddFontFromFileTTF("C:/Users/maxst/Downloads/Procedural-3D-Ray-Marching-Engine/Resources/fonts/mainfont.ttf", 12.0f, nullptr); // smaller font
    font3 = io.Fonts->AddFontFromFileTTF("C:/Users/maxst/Downloads/Procedural-3D-Ray-Marching-Engine/Resources/fonts/mainfont.ttf", 36.0f, nullptr); // larger font
    font4 = io.Fonts->AddFontFromFileTTF("C:/Users/maxst/Downloads/Procedural-3D-Ray-Marching-Engine/Resources/fonts/mainfont.ttf", 6.0f, nullptr); // smallest font

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

}

void mandelbulb(){
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
        ImGui::DragFloat("Time Multiplier", &timeMultiplier, 0.001, 0, 1, nullptr, ImGuiSliderFlags_AlwaysClamp);
    }
    ImGui::Indent(32.0f);

}

void rayMarchingScene(){

    rayMarching = true;
    inEditor = false;

    ImGui::Separator();
    ImGui::Text("Settings");
    ImGui::Separator();
    ImGui::PushFont(font2);
    ImGui::Text("");
    ImGui::PopFont();

    ImGui::BeginChild("Settings Child", ImVec2(screenWidth * 0.358f * 0.99f, screenHeight / 2.5f));

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
        mandelbulb();
    }

    ImGui::Indent(-64.0f);

    ImGui::EndChild();
}

void rayTracingScene(){
    rayMarching = false;

    ImGui::Separator();
    ImGui::Text("Settings");
    ImGui::Separator();
    ImGui::PushFont(font2);
    ImGui::Text("");
    ImGui::PopFont();

    ImGui::BeginChild("Settings Child", ImVec2(screenWidth * 0.358f * 0.99f, screenHeight / 2.5f));

    commonParameters();

    ImGui::EndChild();
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

        if (reflections)
            ImGui::SliderFloat("Visibility", &reflectionVisibility, 0.0f, 1.0f);

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

    if (scene == 5) {
        ImGui::Text("");
        ImGui::SliderFloat(" Smoothness", &smoothness, 0, 1);
    }
}

void menuBar(GLFWwindow* window){
    ImGui::BeginMainMenuBar();

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
        static const char* scenes[]{ "Demo Scene", "Mandelbulb", "Scene Editor", "Cornell Box", "Distance Operations", "Many Entity Test"};
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
        case(5):
            sceneText = " Distance Operations ";
            break;
    }

    auto sceneTextWidth = ImGui::CalcTextSize(sceneText.c_str()).x;

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - sceneTextWidth) * 0.99f);

    ImGui::Text(sceneText.c_str());

    centerText("OpenGL Rendering Engine");

    ImGui::EndMainMenuBar();

}

void fpsCounter(){
    float frameRate = ImGui::GetIO().Framerate;

    ImGui::SetNextWindowPos(ImVec2((float)screenWidth * 0.96f, (float)screenHeight * 0.95f));
    ImGui::SetNextWindowSize(ImVec2((float)screenWidth * 0.139f, (float)screenHeight * 0.02604f));

    //fps counter (bottom right corner)
    if (!paused) {
        ImGui::Begin("FPS", nullptr, window_flags_transparent);
        ImGui::PushFont(font1);
        ImGui::Text("%i", static_cast<int>(frameRate));
        ImGui::PopFont();
        ImGui::End();
    }
}

void fpsCapGui(){
    //FPS cap
    ImGui::Checkbox(" FPS Cap", &fpsCap);

    if (fpsCap){
        ImGui::SameLine();
        ImGui::SliderInt("##foo", &frameLimit, 30, 240);
    }
}

void performanceGraph(float frameRate, float mspf){
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

    //

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
        values2[values_offset2] = mspf;
        values_offset2 = (values_offset2 + 1) % IM_ARRAYSIZE(values2);
        refresh_time2 += 1.0f / 60.0f;
    }

    float average2 = 0.0f;

    for (int n = 0; n < IM_ARRAYSIZE(values2); n++)
        average2 += values2[n];
    average2 /= (float)IM_ARRAYSIZE(values2);

    char overlay2[32];
    sprintf_s(overlay2, "Average: %f", average2);

    ImGui::BeginChild("Graphs", ImVec2(screenWidth * 0.35f, screenHeight / 3), false, window_flags_child);

    centerText("Frame Rate (FPS)");
    ImGui::PlotLines("", values, IM_ARRAYSIZE(values), values_offset, overlay, average + (average / 10), average - (average / 10), ImVec2(screenWidth * 0.35f, 350.0f));

    centerText("Milliseconds Per Frame");
    ImGui::PlotLines("", values2, IM_ARRAYSIZE(values2), values_offset2, overlay2, average2 + (average2 / 10), average2 - (average2 / 10), ImVec2(screenWidth * 0.35f, 330.0f));

    fpsCapGui();

    ImGui::EndChild();
}

void infoMenuText(){
    ImGui::TextWrapped("This program is realtime OpenGL rendering engine that has support for ray marching and ray tracing rendering.");
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

    ImGui::TextWrapped("In the Scene Editor scene, you are able to add and customize new 3D primitives in a ray marching scene, and there are two sections, one for creating a scene and the other for modifying ray marching parameters.");
    ImGui::Text("");

    ImGui::TextWrapped("Made by Max Stalsitz.");
}

#endif