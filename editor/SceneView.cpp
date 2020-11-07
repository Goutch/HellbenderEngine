
#include <imgui.h>
#include "SceneView.h"
#include "HBE.h"

#define INT2VOIDP(i) (void*)(uintptr_t)(i)

const char* SceneView::name="Scene View";
void SceneView::draw(bool& active) {
    ImGui::Begin(name,&active);
    float w,h;
    w=ImGui::GetWindowWidth();
    h=ImGui::GetWindowHeight();
    unsigned int tex_id=Graphics::getRenderTarget()->getTextureID();

    ImGui::GetWindowDrawList()->AddImage(
            INT2VOIDP(tex_id),
            ImVec2(ImGui::GetCursorScreenPos()),
            ImVec2(ImGui::GetCursorScreenPos().x + w,
                   ImGui::GetCursorScreenPos().y +h), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}
