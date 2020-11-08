//
// Created by User on 07-Nov.-2020.
//

#include <imgui.h>
#include "SceneHierarchy.h"
#include "HBE.h"
const char* SceneHierarchy::name="Scene Hierarchy";
std::list<Entity*> SceneHierarchy::selected_entities;
void drawTree(Entity* e)
{
    std::vector<Entity*> children=e->getChildren();
    if(children.empty())
    {
        ImGui::BulletText("%s", e->getName().c_str());
        ImGui::SameLine();
        /*bool selected;
        if(ImGui::Selectable(e->getName().c_str(),selected))
        {
            SceneHierarchy::selected_entities.push_back(e);
        }*/
    }
    else
    {
        bool open=ImGui::TreeNode(e->getName().c_str());
        ImGui::SameLine();
        /*bool selected;
        if(ImGui::Selectable(e->getName().c_str(),))
        {
            SceneHierarchy::selected_entities.push_back(e);
        }*/
        if(open)
        {

            for (auto e:children) {
                drawTree(e);
            }
            ImGui::TreePop();
        }
    }
}
void SceneHierarchy::draw(bool& active) {

    if(active)
    {
        ImGui::Begin(name,&active,ImGuiWindowFlags_NoCollapse);
        std::vector<Entity*> entities=HBE::scene->getEntities();
        for (auto e:entities) {
            if(e->getParent()== nullptr)
            {
                drawTree(e);
            }
        }
        ImGui::End();
    }

}
