#include "Configs.h"

namespace HBE {

    std::string Configs::icon_path = "images/logo/HellbenderLogo.png";

    bool Configs::antialiasing = true;
    Event<bool> Configs::onAntialiasingChange;

    bool Configs::vertical_sync = false;
    Event<> Configs::onVerticalSyncChange;

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

    std::string Configs::getWindowIconPath() {
        return icon_path;
    }

	void Configs::setWindowIconPath(std::string path) {
		icon_path = path;
	}
}
