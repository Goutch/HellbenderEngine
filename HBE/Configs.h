#pragma once

#include "core/utility/Event.h"
#include "Core.h"
namespace HBE {
    class HB_API Configs {
    public:
        enum CAMERA_MODE {
            PERSPECTIVE, ORTHOGRAPHIC
        };
    private:
        static std::string icon_path;
        static std::string window_title;
        static bool antialiasing;
        static bool vertical_sync;
        static CAMERA_MODE default_camera_mode;
        static bool custom_rendering;

    public:
        static Event<std::string> onWindowTitleChange;

        static void setWindowTitle(std::string title);

        static std::string getWindowTitle();


        static Event<bool> onAntialiasingChange;

        static void setAntialiasing(bool antialiasing);

        static bool getAntialiasing();

        static Event<bool> onVerticalSyncChange;

        static void setVerticalSync(bool v_sync);

        static bool getVerticalSync();

        static Event<CAMERA_MODE> onDefaultCameraModeChange;

        static void setDefaultCameraMode(CAMERA_MODE mode);

        static CAMERA_MODE getDefaultCameraMode();

        static std::string getWindowIconPath();

        static void setCustomRendering(bool is_custom);

        static bool isCustonRenderingOn();
    };
}
