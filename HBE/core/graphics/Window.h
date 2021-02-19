#pragma once

#include "Core.h"
#include "core/utility/Event.h"
struct GLFWwindow;
namespace HBE {
    class Window {

    public:
        virtual bool shouldClose() = 0;

        virtual void swapBuffers() = 0;

        virtual void requestClose() = 0;

        virtual ~Window() {};

        static Window *create(int width,int height);

        Event<int, int> onWindowSizeChange;

        //todo: fix this hack
        virtual GLFWwindow* getHandle()=0;

        virtual void getSize(int& width,int& height)=0;
    };
}



