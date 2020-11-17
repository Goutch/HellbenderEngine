//
// Created by User on 07-Nov.-2020.
//

#include <imgui.h>
#include "SceneHierarchy.h"
#include "HBE.h"

const char *SceneHierarchy::name = "Scene Hierarchy";
std::unordered_set<Entity *> SceneHierarchy::selected_entities;

void drawTree(Entity *e) {
    std::vector<Entity *> children = e->getChildren();

    bool selected = SceneHierarchy::selected_entities.find(e)!=SceneHierarchy::selected_entities.end();
    ImGui::TreeAdvanceToLabelPos();
    bool open = ImGui::TreeNodeEx(e->getName().c_str(),ImGuiTreeNodeFlags_OpenOnArrow|
                                  (selected?ImGuiTreeNodeFlags_Selected : ImGuiTreeNodeFlags_None)|
                                  (children.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_None));
    if(!ImGui::IsItemToggledOpen()&&ImGui::IsItemClicked()){

        if(Input::getKey(KEY::LEFT_CONTROL))
        {
            if(SceneHierarchy::selected_entities.find(e)==SceneHierarchy::selected_entities.end())
            {
                SceneHierarchy::selected_entities.emplace(e);
            }
            else
            {
                SceneHierarchy::selected_entities.erase(e);
            }
        } else if( Input::getKey(KEY::LEFT_SHIFT))
        {
            //todo: shift key selection
        } else{
            SceneHierarchy::selected_entities.clear();
            SceneHierarchy::selected_entities.emplace(e);
        }

    }

    if (open) {
        for (auto e:children) {
            drawTree(e);
        }
        ImGui::TreePop();
    }

}

void SceneHierarchy::draw(bool &active) {

    if (active) {
        ImGui::Begin(name, &active, ImGuiWindowFlags_NoCollapse);
        std::vector<Entity *> entities = Application::scene->getEntities();
        for (auto e:entities) {
            if (e->getParent() == nullptr) {
                drawTree(e);
            }
        }
        ImGui::End();
    }

}
