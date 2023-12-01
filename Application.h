
#pragma once

#include "Core.h"
#include "Event.h"
#include "Timer.h"
using namespace utils;
namespace HBE {
	class Window;

	class Scene;

	typedef uint32_t VULKAN_REQUIRED_EXTENSION_FLAGS;
	enum VULKAN_REQUIRED_EXTENSIONS_FLAG {
		VULKAN_REQUIRED_EXTENSION_NONE = 0,
		VULKAN_REQUIRED_EXTENSION_RTX = 1,
		VULKAN_REQUIRED_EXTENSION_DESCRIPTOR_INDEXING = 2,
	};

	enum VULKAN_VERSION {
		VULKAN_VERSION_1_0 = 0,
		VULKAN_VERSION_1_1 = 1,
		VULKAN_VERSION_1_2 = 2,
		VULKAN_VERSION_1_3 = 3,
	};
	struct ApplicationInfo {
		std::string name = "Application";
		VULKAN_REQUIRED_EXTENSION_FLAGS required_extension_flags = VULKAN_REQUIRED_EXTENSION_NONE;
		VULKAN_VERSION vulkan_version = VULKAN_VERSION_1_0;
		uint32_t start_width = 1920;
		uint32_t start_height = 1080;
		bool start_fullscreen = false;
	};

	class HB_API Application {
		static Window *window;
		static Timer *time;
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