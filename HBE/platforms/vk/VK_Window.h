#pragma once

#include "core/graphics/Window.h"

struct GLFWwindow;
namespace HBE {
    class VK_Window : public Window {
        GLFWwindow *handle;
    public:
        VK_Window(int width, int height);

        ~VK_Window();

        bool shouldClose() override;

        void swapBuffers() override;

        void getSize(int &width, int &height) override;

        GLFWwindow *getHandle() override;

        void requestClose() override;

        static void windowSizeCallback(GLFWwindow *handle, int width, int height);
    };
}

