
#pragma once
#include "Core.h"
#include "core/utility/Event.h"
#include "core/scene/Scene.h"

namespace HBE {
    class Window;
    class Clock;
    class HB_API Application {
        static Window *window;
        static Clock *time;
        static int fps_counter;
        static float fps_timer;
    public:
        static Event<> onRegisterComponents;
        static Event<> onInit;
        static Event<float> onUpdate;
        static Event<Scene *> onSceneChange;
        static Event<> onRender;
        static Event<> onWindowClosed;
        static Event<> onQuit;
        static Event<> onDraw;
        static Scene *scene;

        static void init();

        static void run();

        static Scene *setScene(std::string path);

        static void quit();

        static void terminate();

        static float getTime();

    private:
        static void printFPS(float);
        static void registerComponents();


    };


}