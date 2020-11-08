#pragma once

#include "HBE.h"
#include "imgui.h"
struct ViewOptions {
    bool terminal_active = true;
    bool scene_view_active = true;
    bool scene_hierarchy_active = true;
    bool inspector_active=true;
};
class Editor {
    int window_width;
    int window_height;
    void setStyle();
public:

    ViewOptions viewOptions;

    void onWindowSizeChange(int width, int height);

    void start();

    void update(float delta);

    void onRender();

    void terminate();
};
