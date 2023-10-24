#pragma once

#include "core/utility/Event.h"
#include "Core.h"
#include <string>

namespace HBE {
	class HB_API Configs {
	private:
		static std::vector<std::string> icon_paths;
		static bool antialiasing;
		static bool vertical_sync;
		static bool present_automaticaly;

	public:
		static Event<bool> onAntialiasingChange;

		static void setAntialiasing(bool antialiasing);

		static bool getAntialiasing();

		static Event<> onVerticalSyncChange;

		static bool getVerticalSync();

		static void setVerticalSync(bool v_sync);

		static std::vector<std::string> getWindowIconPaths();

		void setWindowIconPaths(std::vector<std::string> paths);


	};
}
