#include "Configs.h"

namespace HBE {
    Event<std::string> Configs::onWindowTitleChange;
    std::string Configs::window_title = "CHANGE WITH Configs::setWindowTitle";

    std::string Configs::icon_path = "../wall.png";

    bool Configs::antialiasing = true;
    Event<bool> Configs::onAntialiasingChange;

    bool Configs::vertical_sync = false;
    Event<> Configs::onVerticalSyncChange;

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
