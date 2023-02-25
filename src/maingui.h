//imgui libaries, for gui
#ifndef MAINGUI_H
#define MAINGUI_H

#include "Source.h"

ImFont* font1;
ImFont* font2;
ImFont* font3;
ImFont* font4;

void styleInitialization(){

    // imgui initialization and customizing the color scheme
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    font1 = io.Fonts->AddFontFromFileTTF("../Resources/fonts/mainfont.ttf", 24.0f, nullptr); // normal font
    font2 = io.Fonts->AddFontFromFileTTF("../Resources/fonts/mainfont.ttf", 12.0f, nullptr); // smaller font
    font3 = io.Fonts->AddFontFromFileTTF("../Resources/fonts/mainfont.ttf", 36.0f, nullptr); // larger font
    font4 = io.Fonts->AddFontFromFileTTF("../Resources/fonts/mainfont.ttf", 6.0f, nullptr); // smallest font

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

    if (scene == 5) {
        ImGui::Text("");
        ImGui::SliderFloat(" Smoothness", &smoothness, 0, 1);
    }
}

#endif

