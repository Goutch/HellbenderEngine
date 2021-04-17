#pragma once

#include "core/graphics/Window.h"

struct GLFWwindow;
namespace HBE {
    class GL_Window : public Window {
        GLFWwindow *handle;
    public:
        GL_Window(int width, int height);

        ~GL_Window();


        bool shouldClose() override;

        void getSize(int &width, int &height) override;

        void swapBuffers() override;

        void requestClose() override;

        GLFWwindow *getHandle() override;

        void onVerticalSyncChange(bool v_sync);

        void onTitleChange(std::string title);

        static void windowSizeCallback(GLFWwindow *handle, int width, int height);
    };

}

