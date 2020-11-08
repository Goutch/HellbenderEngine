//
// Created by User on 07-Nov.-2020.
//

#include "Inspector.h"
#include "imgui.h"
const char *Inspector::name = "Inspector";

void Inspector::draw(bool active) {
    if(active)
    {
        ImGui::Begin(name,&active,ImGuiWindowFlags_NoCollapse);

        ImGui::End();
    }
}
