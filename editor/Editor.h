#pragma once

#include "HBE.h"
struct ViewOptions {
    bool terminal_active = true;
    bool scene_view_active = true;
    bool scene_hierarchy_active = true;
};
class Editor {
    int window_width;
    int window_height;




public:

    ViewOptions viewOptions;
    void onWindowSizeChange(int width, int height);

    void start();

    void update(float delta);

    void onRenderFinish(RenderTarget *renderTarget);

    void terminate();
};
