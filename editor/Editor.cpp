//
// Created by user on 11/6/20.
//

#include "Editor.h"
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

void Editor::start() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    ImGui_ImplGlfw_InitForOpenGL(Graphics::getWindow(), true);
    ImGui_ImplOpenGL3_Init("3.3");
    ImGui::StyleColorsDark();
    HBE::onUpdate.subscribe(this,&Editor::update);
    HBE::onRenderFinish.subscribe(this,&Editor::onRenderFinish);
}

void Editor::update(float delta) {
    ImGui::Begin("Demo window");
    ImGui::Button("Hello!");
    ImGui::End();
}

void Editor::onRenderFinish(RenderTarget* renderTarget) {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
