#include "Configs.h"

namespace HBE {

	std::vector<std::string> Configs::icon_paths = {"images/logo/HellbenderLogo128x128.png", "images/logo/HellbenderLogo64x64.png", "images/logo/HellbenderLogo32x32.png", "images/logo/HellbenderLogo16x16.png"};
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

	std::vector<std::string> Configs::getWindowIconPaths() {
		return icon_paths;
	}

	void Configs::setWindowIconPaths(std::vector<std::string> paths) {
		icon_paths = paths;
	}
}
