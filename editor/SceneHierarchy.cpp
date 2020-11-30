//
// Created by User on 07-Nov.-2020.
//

#include <imgui.h>
#include "SceneHierarchy.h"
#include "HBE.h"

using namespace HBE;
const char *SceneHierarchy::name = "Scene Hierarchy";
std::unordered_set<Entity *> SceneHierarchy::selected_entities;
std::list<Entity *> SceneHierarchy::delete_query;

void drawNewEntityMenu(Transform *parent) {
    if (ImGui::BeginMenu("New")) {
        if (ImGui::MenuItem("Empty"))
            Application::scene->instantiate()->transform->setParent(parent);
        if (ImGui::MenuItem("Cube")) {
            MeshRenderer *mr = Application::scene->instantiate<MeshRenderer>();
            mr->entity->transform->setParent(parent);
            mr->setMesh(*Graphics::DEFAULT_CUBE);
            mr->setMaterial(*Graphics::DEFAULT_MESH_MATERIAL);
        }
        ImGui::EndMenu();
    }
}

void SceneHierarchy::drawTree(Entity *e) {
    std::vector<Transform *> children = e->transform->getChildren();

    bool selected = selected_entities.find(e) != selected_entities.end();
    ImGui::TreeAdvanceToLabelPos();
    bool open = ImGui::TreeNodeEx(e->getName().c_str(), ImGuiTreeNodeFlags_OpenOnArrow |
                                                        ImGuiTreeNodeFlags_SpanAvailWidth |
                                                        (selected ? ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None) |
                                                        (children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None));
    if (!ImGui::IsItemToggledOpen() && ImGui::IsItemClicked()) {

        if (Input::getKey(KEY::LEFT_CONTROL)) {
            if (selected_entities.find(e) == selected_entities.end()) {
                selected_entities.emplace(e);
            } else {
                selected_entities.erase(e);
            }
        } else if (Input::getKey(KEY::LEFT_SHIFT)) {
            //todo: shift key selection
        } else {
            selected_entities.clear();
            selected_entities.emplace(e);
        }

    }
    if (ImGui::BeginPopupContextItem()) {
        drawNewEntityMenu(e->transform);
        if (ImGui::MenuItem("Delete")) {
            if (!selected) {
                delete_query.push_back(e);
            } else {
                for (auto selected_entity :SceneHierarchy::selected_entities)
                    delete_query.push_back(selected_entity);
                SceneHierarchy::selected_entities.clear();
            }
        }
        ImGui::EndPopup();
        if (open)
            ImGui::TreePop();
    } else if (open) {
        for (auto transform:children) {
            drawTree(transform->entity);
        }
        ImGui::TreePop();
    }

}

void SceneHierarchy::draw(bool &active) {
    if (active) {
        ImGui::Begin(name, &active, ImGuiWindowFlags_NoCollapse);
        std::vector<Entity *> entities = Application::scene->getSceneTree();
        for (auto e:entities) {
            if (e->transform->getParent() == nullptr)
                drawTree(e);
        }

        if (ImGui::BeginPopupContextWindow(0, 1, false)) {
            drawNewEntityMenu(nullptr);
            ImGui::EndPopup();
        }
        ImGui::End();
    }

}

void SceneHierarchy::update() {
    for (Entity *e :delete_query) {
        Application::scene->destroy(e);
    }
    delete_query.clear();
}
