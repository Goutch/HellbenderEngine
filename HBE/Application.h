
#pragma once
#include <Core.h>
#include "core/utility/Event.h"
#include "core/entity/Scene.h"
class Clock;

struct GLFWwindow;

class HB_API Application {
    static GLFWwindow *window;
    static Clock* time;
    static int fps_counter;
    static float fps_timer;

public:
    static Event<> onInit;
    static Event<float> onUpdate;
    static Event<Scene*> onSceneChange;
    static Event<> onRender;

    static Scene *scene;

    static void init();

    static void run();

    static Scene *setScene(std::string path);

    static void quit();

    static void terminate();

    static float getTime();
private:
    static void printFPS(float);
};

