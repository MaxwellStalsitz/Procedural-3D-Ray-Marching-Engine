#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include "Source.h"
#include "maingui.h"

void sceneEditorGui(){
    inEditor = true;

    ImGui::SetNextItemOpen(true);

    ImGui::Separator();
    ImGui::Text("Scene Hierarchy");
    ImGui::Separator();

    ImGui::BeginChild("Scene Heirarchy Child", ImVec2(screenWidth * 0.358f * 0.99f, screenHeight / 2.75f));

    //rendering heirarchy system through imgui
    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode("Entities:"))
    {
        ImGuiTreeNodeFlags node_flags = window_flags_parameters;

        node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen; // ImGuiTreeNodeFlags_Bullet

        for (int i = 0; i < numberOfEntities; i++) {
            std::string nodeName = " " + sceneArray[i].name;
            ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, nodeName.c_str(), i);
            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
                node_clicked = i;
                changedPrimitive = sceneArray[node_clicked].shape;
            }
        }

        ImGui::TreePop();
    }
    ImGui::EndChild();

    ImGui::Text("");

    ImGui::Separator();
    ImGui::Text("Entity Editing");
    ImGui::Separator();

    ImGui::BeginChild("Entity Editing", ImVec2(screenWidth * 0.358f * 0.99f, screenHeight / 8));

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

        if (node_clicked != -1) { // need to re-iterate in the case that entity is deleted
            ImGui::SameLine();

            static const char* primitives[]{ "Sphere", "Box", "Torus", "Octahedron", "Round Box", "Box Frame" };
            ImGui::SetNextItemWidth(screenWidth * 0.285f);

            ImGui::Combo("##foo", &changedPrimitive, primitives, IM_ARRAYSIZE(primitives));

            if (sceneArray[node_clicked].shape != changedPrimitive)
                sceneArray[node_clicked].shape = changedPrimitive;
        }
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

    ImGui::PushItemWidth(screenWidth / 2.85f);
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

    float materialTextYPos = ImGui::GetCursorPosY();

    ImGui::SameLine();

    ImGui::PushFont(font3); // larger font size
    std::string addText = " Create Entity ";
    auto textWidth = ImGui::CalcTextSize(addText.c_str()).x;
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 1.25f);

    if (ImGui::Button(addText.c_str()) && (entityName[0] != '\0')) {
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

    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - textWidth) / 1.25f);

    static const char* primitives[]{ "Sphere", "Box", "Torus", "Octahedron", "Round Box", "Box Frame" };
    ImGui::SetNextItemWidth(screenWidth * 0.285f);
    ImGui::Combo("##foo", &primitiveSelected, primitives, IM_ARRAYSIZE(primitives));

    // ------------------------------------------------------------------------
    //imgui color picker for material, code from imgui demo scene

    //
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - (screenWidth * 0.02605f) ) * 0.15f);
    ImGui::SetCursorPosY((materialTextYPos));
    //

    if (ImGui::ColorButton("Material Color", color, window_flags_editor, ImVec2((screenWidth * 0.02605f), (screenHeight * 0.0463f)))) {
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
}

#endif
