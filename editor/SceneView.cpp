
#include <imgui.h>
#include "SceneView.h"
#include "HBE.h"
float SceneView::last_window_width=0;
float SceneView::last_window_height=0;
const char* SceneView::name="Scene View";
void SceneView::draw(bool& active) {
    if(active)
    {
        ImGui::Begin(name,&active,ImGuiWindowFlags_NoCollapse);
        float w,h;
        w=ImGui::GetWindowWidth();
        h=ImGui::GetWindowHeight();
        if(abs(last_window_width-w)>0.0000001f||abs(last_window_height-h)>0.0000001f)
        {
            Graphics::getRenderTarget()->setSize(w,h);
        }

        unsigned int tex_id=Graphics::getRenderTarget()->getTextureID();

        ImGui::GetWindowDrawList()->AddImage(
                (void*)(uintptr_t)tex_id,
                ImVec2(ImGui::GetCursorScreenPos()),
                ImVec2(ImGui::GetCursorScreenPos().x + w,
                       ImGui::GetCursorScreenPos().y +h), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
}
