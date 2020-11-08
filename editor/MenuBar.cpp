//
// Created by User on 07-Nov.-2020.
//

#include <imgui.h>
#include "MenuBar.h"
#include "Editor.h"

#include "SceneHierarchy.h"
#include "SceneView.h"
#include "Terminal.h"
void MenuBar::draw(ViewOptions& viewOptions) {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {

            }
            if (ImGui::MenuItem("New")) {

            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::RadioButton(SceneHierarchy::name,viewOptions.scene_hierarchy_active)) {
                viewOptions.scene_hierarchy_active=!viewOptions.scene_hierarchy_active;
            }
            if (ImGui::RadioButton(SceneView::name,viewOptions.scene_view_active)) {
                viewOptions.scene_view_active=!viewOptions.scene_view_active;
            }
            if (ImGui::RadioButton(Terminal::name,viewOptions.terminal_active)) {
                viewOptions.terminal_active=!viewOptions.terminal_active;
            }
            if (ImGui::RadioButton(Terminal::name,viewOptions.inspector_active)) {
                viewOptions.inspector_active=!viewOptions.inspector_active;
            }
            ImGui::EndMenu();
        }

    }
    ImGui::EndMenuBar();
}


