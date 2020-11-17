
#include "Editor.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLAD

#include "HBE.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "MenuBar.h"
#include "Terminal.h"
#include "SceneView.h"
#include "imgui_internal.h"
#include "SceneHierarchy.h"
#include "Inspector.h"

using namespace ImGui;


void Editor::start() {

    Configs::setCustomRendering(true);
    IMGUI_CHECKVERSION();
    CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(Graphics::getWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
    setStyle();
    Application::onRender.subscribe(this, &Editor::onRender);
    Graphics::onWindowSizeChange.subscribe(this, &Editor::onWindowSizeChange);
    Graphics::getWindowSize(window_width, window_height);
    Configs::setWindowTitle("Editor");

}


void Editor::onRender() {
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
            ImGui::DockBuilderDockWindow(Inspector::name, dock_id_right);
            ImGui::DockBuilderDockWindow(SceneView::name, dock_main_id);
            ImGui::DockBuilderFinish(dockspace_id);
        }
        DockSpace(dockspace_id);

        MenuBar::draw(viewOptions);
        Terminal::draw(viewOptions.terminal_active);
        SceneView::draw(viewOptions.scene_view_active);
        SceneHierarchy::draw(viewOptions.scene_hierarchy_active);
        Inspector::draw(viewOptions.inspector_active);
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

void Editor::setStyle() {
    StyleColorsDark();
#define HI   ImVec4(0.502f, 0.075f, 0.256f, 1)
#define MED  ImVec4(0.455f, 0.198f, 0.301f, 1)
#define LOW  ImVec4(0.355f, 0.201f, 0.271f, 1)

#define LOW_BG ImVec4(0.100f, 0.120f, 0.170f, 1)
#define BG  ImVec4(0.200f, 0.220f, 0.270f, 1)
#define HI_BG  ImVec4(0.300f, 0.320f, 0.370f, 1)

#define TEXT ImVec4(0.860f, 0.930f, 0.890f, 1)

#define INVISIBLE ImVec4(0,0,0,0)
    auto &style = ImGui::GetStyle();


    style.Colors[ImGuiCol_Tab]                      = MED;
    style.Colors[ImGuiCol_TabActive]                = HI;
    style.Colors[ImGuiCol_TabUnfocusedActive]       = MED;
    style.Colors[ImGuiCol_TabUnfocused]             = MED;
    style.Colors[ImGuiCol_TabHovered]               = HI;

    style.Colors[ImGuiCol_TitleBg]                  = BG;
    style.Colors[ImGuiCol_TitleBgActive]            = LOW;
    style.Colors[ImGuiCol_TitleBgCollapsed]         = BG;

    style.Colors[ImGuiCol_Border]                   = MED;
    style.Colors[ImGuiCol_BorderShadow]             = INVISIBLE;

    style.Colors[ImGuiCol_Text]                     = TEXT;
    style.Colors[ImGuiCol_TextDisabled]             = TEXT;
    style.Colors[ImGuiCol_WindowBg]                 = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    //style.Colors[ImGuiCol_ChildWindowBg]            = BG( 0.58f);
    style.Colors[ImGuiCol_PopupBg]                  = BG;

    style.Colors[ImGuiCol_FrameBg]                  = BG;
    style.Colors[ImGuiCol_FrameBgHovered]           = MED;
    style.Colors[ImGuiCol_FrameBgActive]            = MED;
    style.Colors[ImGuiCol_MenuBarBg]                = BG;
    style.Colors[ImGuiCol_ScrollbarBg]              = BG;
    style.Colors[ImGuiCol_ScrollbarGrab]            = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]     = MED;
    style.Colors[ImGuiCol_ScrollbarGrabActive]      = HI;
    style.Colors[ImGuiCol_CheckMark]                = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]               = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]         = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_Button]                   = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]            = MED;
    style.Colors[ImGuiCol_ButtonActive]             = HI;
    style.Colors[ImGuiCol_Header]                   = LOW;
    style.Colors[ImGuiCol_HeaderHovered]            = MED;
    style.Colors[ImGuiCol_HeaderActive]             = HI;
    //style.Colors[ImGuiCol_Column]                   = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    //style.Colors[ImGuiCol_ColumnHovered]            = MED;
    //style.Colors[ImGuiCol_ColumnActive]             = MED;
    style.Colors[ImGuiCol_ResizeGrip]               = LOW;
    style.Colors[ImGuiCol_ResizeGripHovered]        = MED;
    style.Colors[ImGuiCol_ResizeGripActive]         = HI;
    style.Colors[ImGuiCol_PlotLines]                = TEXT;
    style.Colors[ImGuiCol_PlotLinesHovered]         = MED;
    style.Colors[ImGuiCol_PlotHistogram]            = TEXT;
    style.Colors[ImGuiCol_PlotHistogramHovered]     = MED;
    style.Colors[ImGuiCol_TextSelectedBg]           = MED;
    style.Colors[ImGuiCol_ModalWindowDarkening]     = BG;

    style.WindowPadding            = ImVec2(6, 4);
    style.WindowRounding           = 0.0f;
    style.FramePadding             = ImVec2(5, 2);
    style.FrameRounding            = 3.0f;
    style.ItemSpacing              = ImVec2(7, 1);
    style.ItemInnerSpacing         = ImVec2(1, 1);
    style.TouchExtraPadding        = ImVec2(0, 0);
    style.IndentSpacing            = 6.0f;
    style.ScrollbarSize            = 12.0f;
    style.ScrollbarRounding        = 16.0f;
    style.GrabMinSize              = 20.0f;
    style.GrabRounding             = 2.0f;

    style.WindowTitleAlign.x = 0.50f;


    style.FrameBorderSize = 0.0f;
    style.WindowBorderSize = 0.0f;
}
