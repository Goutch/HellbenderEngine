#pragma once

#include "Context.h"
#include "Core.h"
#include "audio/Audio.h"
#include "dependencies/utils-collection/Event.h"
#include "dependencies/utils-collection/Timer.h"
#include "graphics/Window.h"
#include "input/Input.h"

namespace HBE
{
    struct ApplicationInfo
    {
        std::string app_name;
        WindowInfo window_info{};
        ContextInfo context_info{};
    };

    class HB_API Application
    {
        Window* window = nullptr;
        Context* context = nullptr;
        //todo: change Input static class to here
        Audio audio;
        Input input;

        ApplicationInfo info;
        Timer time;
        int fps_counter = 0;
        float fps_timer = 0.0f;

    public:
        static Application* instance;
        Event<> onRegisterComponents;
        Event<> onInit;
        Event<> onWindowClosed;
        Event<> onQuit;

        Event<float> onUpdate;
        Event<> onDraw;
        Event<> onRender;
        Event<> onPresent;
        Event<uint64_t> onFrameEnd;
        Event<uint64_t> onFrameStart;

        void init(ApplicationInfo& info);
        void run();
        Input* getInput();
        Application() = default;
        Window* getWindow();
        Context* getContext();

        void quit();

        void terminate();

        float getTime();

        const ApplicationInfo& getInfo();

    private:
        void printFPS(float);
    };
}
