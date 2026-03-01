#pragma once

#include "Core.h"
#include "dependencies/utils-collection/Event.h"
#include "HBETypes.h"
#include "core/input/Input.h"

struct GLFWwindow;

namespace HBE
{
    struct WindowInfo
    {
        vec2i startSize = vec2i(1920, 1080);
        bool fullscreen = false;
    };

    class HB_API Window
    {
    protected:
        bool is_fullscreen = false;

    public:
        Input input;
        /// <summary>
        /// Called when the window is resized.
        /// Do not use this to edit graphics resources because it is called before the current frame is finished rendering.
        /// Use Graphics::onSwapchainResized instead, it is called after the current frame is finished rendering
        /// </summary>
        Event<Window*> onSizeChange;

        virtual bool shouldClose() = 0;

        virtual void requestClose() = 0;

        bool isFullscreen();

        virtual ~Window() = default;

        //todo: fix this hack
        virtual GLFWwindow* getHandle() = 0;

        virtual void getSize(uint32_t& width, uint32_t& height) = 0;

        virtual uint32_t getWidth() = 0;

        virtual uint32_t getHeight() = 0;

        virtual void setFullscreen(bool fullscreen) = 0;

        virtual bool isMinimized() = 0;
    };
}
