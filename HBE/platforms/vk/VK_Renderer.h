#pragma once
#include "core/graphics/Renderer.h"
namespace HBE{
    class VK_Renderer: public Renderer {
        GLFWwindow* window;
    public:
        ~VK_Renderer();
        GLFWwindow *createWindow(int32 width, int32 height) override;
    };
}
