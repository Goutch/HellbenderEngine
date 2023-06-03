
#pragma once

#include "Core.h"
#include "core/utility/Event.h"

namespace HBE {
	class Window;

	class Clock;

	class Scene;

	typedef uint32_t HARDWARE_FLAGS;
	enum HARDWARE_FLAG {
		HARDWARE_FLAG_NONE = 0,
		HARDWARE_FLAG_GPU_REQUIRE_VULKAN_1_2 = 1 << 0,
		HARDWARE_FLAG_GPU_REQUIRE_VULKAN_1_3 = 1 << 1,
		HARDWARE_FLAG_GPU_REQUIRE_RTX_CAPABILITIES = 1 << 2,
		HARDWARE_FLAG_GPU_REQUIRE_DESCRIPTOR_INDEXING_CAPABILITIES = 1 << 3,
	};
	struct ApplicationInfo {
		std::string name = "Application";
		HARDWARE_FLAGS hardware_flags = HARDWARE_FLAG_NONE;
		uint32_t start_width = 1920;
		uint32_t start_height = 1080;
		bool start_fullscreen = false;
	};

	class HB_API Application {
		static Window *window;
		static Clock *time;
		static int fps_counter;
		static float fps_timer;
		static ApplicationInfo info;
	public:
		static Event<> onRegisterComponents;
		static Event<> onInit;
		static Event<> onWindowClosed;
		static Event<> onQuit;

		static Event<float> onUpdate;
		static Event<> onDraw;
		static Event<> onRender;
		static Event<> onPresent;

		static void init(const ApplicationInfo &info);

		static void run();

		static void quit();

		static void terminate();

		static float getTime();

		static const ApplicationInfo &getInfo();

	private:
		static void printFPS(float);


	};


}