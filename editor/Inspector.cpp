
#include "Inspector.h"
#include "SceneHierarchy.h"
#include "imgui.h"

const char *Inspector::name = "Inspector";

void Inspector::draw(bool active) {
    if (active) {
        ImGui::Begin(name, &active, ImGuiWindowFlags_NoCollapse);
        if (!SceneHierarchy::selected_entities.empty()) {
            int count = 0;
            for (Entity *e:SceneHierarchy::selected_entities) {
                ImGui::BeginGroup();
                if (ImGui::CollapsingHeader(e->getName().c_str()), ImGuiTreeNodeFlags_DefaultOpen|ImGuiTreeNodeFlags_Framed) {
                    for (Component *c:e->getComponents()) {
                        ImGui::BeginGroup();
                        if (ImGui::CollapsingHeader(c->toString().c_str()), ImGuiTreeNodeFlags_DefaultOpen|ImGuiTreeNodeFlags_Framed) {
                            ImGui::Text("patate");
                        }
                        ImGui::EndGroup();
                    }
                }
                ImGui::EndGroup();
                count++;
            }
        }
        ImGui::End();
    }
}
