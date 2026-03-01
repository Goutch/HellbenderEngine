#include "Application.h"

#include <thread>

#include "core/input/Input.h"
#include "GLFW/glfw3.h"
#include "core/graphics/Window.h"
#include "dependencies/utils-collection/Profiler.h"
#include "core/audio/Audio.h"

#include "platforms/vk/VK_Window.h"
#include "platforms/vk/VK_Context.h"

namespace HBE
{
    Application* Application::instance = nullptr;

    void Application::init(ApplicationInfo& info)
    {
        HB_ASSERT(instance == nullptr, "Application is a singleton, there can only be one instance");
        instance = this;
        Application::info = info;
        switch (info.context_info.api)
        {
        case GRAPHICS_API_VULKAN:
            window = new VK_Window(info.window_info);
            context = new VK_Context(info.context_info);
            break;
        }
        audio.init();
        input.init(*window);

        onInit.invoke();
    }

    void Application::run()
    {
        time = new Timer();
        Timer update_clock = Timer();
        float delta_t = 0.0f;
        uint64_t frames = 0;
        while (!window->shouldClose())
        {
            HB_PROFILE_BEGIN("TOTAL_FRAME");
            onFrameStart.invoke(frames);
            input.pollEvents();

            HB_PROFILE_BEGIN("UPDATE");
            onUpdate.invoke(delta_t);
            HB_PROFILE_END("UPDATE");

            if (!window->isMinimized())
            {
                HB_PROFILE_BEGIN("DRAW");
                onDraw.invoke();
                HB_PROFILE_END("DRAW");


                HB_PROFILE_BEGIN("RENDER");
                context->getRenderer()->beginFrame();
                onRender.invoke();
                HB_PROFILE_END("RENDER");
                HB_PROFILE_BEGIN("PRESENT");
                onPresent.invoke();
                context->getRenderer()->endFrame();

                HB_PROFILE_END("PRESENT");
            }
            else if (window->isMinimized())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(33));
            }


            delta_t = update_clock.ns() * NANOSECONDS_TO_SECONDS;
            update_clock.reset();
            printFPS(delta_t);
            onFrameEnd.invoke(frames);
            frames++;
            HB_PROFILE_END("TOTAL_FRAME");
        }
        context->getRenderer()->waitLastFrame();
        onWindowClosed.invoke();
        onQuit.invoke();
        delete time;
    }

    Input* Application::getInput()
    {
        return &input;
    }

    Window* Application::getWindow()
    {
        return window;
    }

    Context* Application::getContext()
    {
        return context;
    }

    void Application::terminate()
    {
        delete window;
        delete context;
        Profiler::printAverange();
    }

    void Application::quit()
    {
        window->requestClose();
    }

    float Application::getTime()
    {
        if (time)
        {
            return time->ns() * NANOSECONDS_TO_SECONDS;
        }
        return 0;
    }

    void Application::printFPS(float delta)
    {
        fps_counter++;
        fps_timer += delta;
        if (fps_timer >= 1.0)
        {
            Log::debug("fps:" + std::to_string(fps_counter) + "(" + std::to_string(1000.0 / fps_counter) + "ms)");
            fps_counter = 0;
            fps_timer = 0;
        }
    }

    const ApplicationInfo& Application::getInfo()
    {
        return info;
    }
}
