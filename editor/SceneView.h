#pragma once
#include "string"
class SceneView {
    static float last_window_width;
    static float last_window_height;
public:
    static const char* name;
    static void draw(bool& active);
};

