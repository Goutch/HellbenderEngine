//
// Created by user on 11/6/20.
//

#include "Editor.h"


#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "MenuBar.h"
#include "Terminal.h"
#include "SceneView.h"
using namespace ImGui;
#include "imgui_internal.h"
#include "SceneHierarchy.h"

void Editor::start() {

    Configs::setCustomRendering(true);
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(Graphics::getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    StyleColorsDark();
    GetStyle().WindowRounding = 0.0f;
    GetStyle().ChildRounding = 0.0f;
    GetStyle().FrameRounding = 0.0f;
    GetStyle().GrabRounding = 0.0f;
    GetStyle().PopupRounding = 0.0f;
    GetStyle().ScrollbarRounding = 0.0f;
    HBE::onRenderFinish.subscribe(this, &Editor::onRenderFinish);
    Graphics::onWindowSizeChange.subscribe(this, &Editor::onWindowSizeChange);
    Graphics::getWindowSize(window_width, window_height);
    Configs::setWindowTitle("Editor");

}


void Editor::onRenderFinish(RenderTarget *renderTarget) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    NewFrame();

    ImGuiID dockspace_id;
    bool show = false;
    SetNextWindowBgAlpha(1.0);
    SetNextWindowPos(ImVec2(.0f, .0f), ImGuiCond_Always);
    SetNextWindowSize(ImVec2(window_width+2, window_height+2), ImGuiCond_Always);
    if(Begin("Editor", &show,
                 ImGuiWindowFlags_NoTitleBar |
                 ImGuiWindowFlags_MenuBar|
                 ImGuiWindowFlags_NoResize |
                 ImGuiWindowFlags_NoMove |
                 ImGuiWindowFlags_NoCollapse))
    {
        dockspace_id = GetID("DockSpace");
        if (ImGui::DockBuilderGetNode(dockspace_id) == NULL)
        {
            ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
            ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
            ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(window_width,window_height));

            ImGuiID dock_main_id = dockspace_id;
            ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
            ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.20f, NULL, &dock_main_id);

            ImGui::DockBuilderDockWindow(Terminal::name, dock_id_bottom);
            ImGui::DockBuilderDockWindow(SceneHierarchy::name, dock_id_left);
            ImGui::DockBuilderDockWindow(SceneView::name, dock_main_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        DockSpace(dockspace_id);

        MenuBar::draw(viewOptions);
        DockBuilderAddNode();
        Terminal::draw(viewOptions.terminal_active);
        DockBuilderAddNode();
        SceneView::draw(viewOptions.scene_view_active);
        DockBuilderAddNode();
        SceneHierarchy::draw(viewOptions.scene_hierarchy_active);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::terminate() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Editor::onWindowSizeChange(int width, int height) {
    this->window_width = width;
    this->window_height = height;
}
