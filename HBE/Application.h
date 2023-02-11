
#pragma once

#include "Core.h"
#include "core/utility/Event.h"

namespace HBE {
	class Window;

	class Clock;

	class Scene;

	class HB_API Application {
		static Window *window;
		static Clock *time;
		static int fps_counter;
		static float fps_timer;
	public:
		static Event <> onRegisterComponents;
		static Event <> onInit;
		static Event<Scene *> onSceneChange;
		static Event <> onWindowClosed;
		static Event <> onQuit;

		static Event<float> onUpdate;
		static Event <> onDraw;
		static Event <> onRender;
		static Event <> onPresent;

		static void init();

		static void run();

		static void quit();

		static void terminate();

		static float getTime();

	private:
		static void printFPS(float);


	};


}