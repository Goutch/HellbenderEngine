#include "Configs.h"

namespace HBE {
    Event<std::string> Configs::onWindowTitleChange;
    std::string Configs::window_title = "CHANGE WITH Configs::setWindowTitle";

    std::string Configs::icon_path = "../wall.png";

    bool Configs::antialiasing = true;
    Event<bool> Configs::onAntialiasingChange;

    bool Configs::vertical_sync = true;
    Event<> Configs::onVerticalSyncChange;

    Configs::CAMERA_MODE Configs::default_camera_mode = Configs::CAMERA_MODE::PERSPECTIVE;
    Event<Configs::CAMERA_MODE> Configs::onDefaultCameraModeChange;

    bool Configs::present_automaticaly = false;

    void Configs::setAntialiasing(bool antialiasing) {
        Configs::antialiasing = antialiasing;
        onAntialiasingChange.invoke(antialiasing);
    }

    bool Configs::getAntialiasing() {
        return antialiasing;
    }

    bool Configs::getVerticalSync() {
        return vertical_sync;
    }

    void Configs::setVerticalSync(bool v_sync) {
		vertical_sync = v_sync;
        onVerticalSyncChange.invoke();
    }

    Configs::CAMERA_MODE Configs::getDefaultCameraMode() {
        return default_camera_mode;
    }

    void Configs::setDefaultCameraMode(CAMERA_MODE mode) {
        default_camera_mode = mode;
        onDefaultCameraModeChange.invoke(default_camera_mode);
    }

    void Configs::setWindowTitle(std::string title) {
        window_title = title;
        onWindowTitleChange.invoke(title);
    }

    std::string Configs::getWindowTitle() {
        return window_title;
    }

    std::string Configs::getWindowIconPath() {
        return icon_path;
    }

    void Configs::setPresentAutomatic(bool is_automatic) {
        present_automaticaly = is_automatic;
    }

    bool Configs::isPresentAutomatic() {
        return present_automaticaly;
    }

}
