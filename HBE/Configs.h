#pragma once

#include "core/utility/Event.h"

class Configs {
public:
    enum CAMERA_MODE {PERSPECTIVE,ORTHOGRAPHIC};
private:
    static bool antialiasing;
    static bool vertical_sync;
    static CAMERA_MODE default_camera_mode;
public:
    static Event<bool> onAntialiasingChange;
    static void setAntialiasing(bool antialiasing);
    static bool getAntialiasing();

    static Event<bool> onVerticalSyncChange;
    static void setVerticalSync(bool v_sync);
    static bool getVerticalSync();

    static Event<CAMERA_MODE> onDefaultCameraModeChange;
    static void setDefaultCameraMode(CAMERA_MODE mode);
    static CAMERA_MODE getDefaultCameraMode();
};
