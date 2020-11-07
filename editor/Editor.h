#pragma once

#include "HBE.h"
class Editor {
public:
    void start();
    void update(float delta);
    void onRenderFinish(RenderTarget* renderTarget);
    void terminate();
};
