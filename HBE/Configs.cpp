#include "Configs.h"

bool Configs::vertical_sync = true;
Event<bool> Configs::onVerticalSyncChange;
Configs::CAMERA_MODE Configs::default_camera_mode = Configs::CAMERA_MODE::PERSPECTIVE;
Event<Configs::CAMERA_MODE> Configs::onDefaultCameraModeChange;
bool Configs::getVerticalSync() {
    return vertical_sync;
}

void Configs::setVerticalSync(bool v_sync) {
    onVerticalSyncChange.invoke(v_sync);
    vertical_sync = v_sync;
}

Configs::CAMERA_MODE Configs::getDefaultCameraMode() {
    return default_camera_mode;
}

void Configs::setDefaultCameraMode(CAMERA_MODE mode) {
    default_camera_mode=mode;
    onDefaultCameraModeChange.invoke(default_camera_mode);
}
