
#include "Application.h"
#include "core/graphics/Graphics.h"
#include "core/input/Input.h"
#include "GLFW/glfw3.h"
#include <core/graphics/Window.h>
#include "core/utility/Profiler.h"
#include "core/scene/Scene.h"
#include "core/audio/Audio.h"
#include <thread>

namespace HBE {
	ApplicationInfo Application::info = {};
	Window *Application::window = nullptr;
	Timer *Application::time = nullptr;
	int Application::fps_counter = 0;
	float Application::fps_timer = 0;
	Event<> Application::onInit = Event<>();
	Event<> Application::onWindowClosed = Event<>();
	Event<> Application::onQuit = Event<>();

	Event<float> Application::onUpdate;
	Event<> Application::onDraw;
	Event<> Application::onRender;
	Event<> Application::onPresent;

	void Application::init(const ApplicationInfo &info) {
		Application::info = info;
		Graphics::init(info.name.c_str(), info.start_width, info.start_height, info.start_fullscreen);
		Audio::init();
		Input::init();
		window = Graphics::getWindow();
		onInit.invoke();
	}

	void Application::run() {

		time = new Timer();
		Timer update_clock = Timer();
		float delta_t = 0.0f;
		while (!window->shouldClose()) {
			window->swapBuffers();
			Input::pollEvents();
			HB_PROFILE_BEGIN("TOTAL_FRAME");
			HB_PROFILE_BEGIN("UPDATE");
			onUpdate.invoke(delta_t);
			HB_PROFILE_END("UPDATE");

			if (!window->isMinimized()) {
				HB_PROFILE_BEGIN("DRAW");
				onDraw.invoke();
				HB_PROFILE_END("DRAW");


				HB_PROFILE_BEGIN("RENDER");
				Graphics::beginFrame();
				onRender.invoke();
				HB_PROFILE_END("RENDER");
				HB_PROFILE_BEGIN("PRESENT");
				onPresent.invoke();
				Graphics::endFrame();
				HB_PROFILE_END("PRESENT");
			} else if (window->isMinimized()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(33));
			}

			HB_PROFILE_END("TOTAL_FRAME");
			delta_t = update_clock.ns() * NANOSECONDS_TO_SECONDS;
			update_clock.reset();
			printFPS(delta_t);
		}
		Graphics::waitCurrentFrame();
		onWindowClosed.invoke();
		onQuit.invoke();
		delete time;
	}

	void Application::terminate() {
		Audio::terminate();
		Graphics::terminate();
		Profiler::printAverange();
	}

	void Application::quit() {
		window->requestClose();
	}

	float Application::getTime() {
		if (time) {
			return time->ns() * NANOSECONDS_TO_SECONDS;
		}
		return 0;
	}

	void Application::printFPS(float delta) {
		fps_counter++;
		fps_timer += delta;
		if (fps_timer >= 1.0) {
			Log::debug("fps:" + std::to_string(fps_counter) + "(" + std::to_string(1000.0 / fps_counter) + "ms)");
			fps_counter = 0;
			fps_timer = 0;
		}
	}

	const ApplicationInfo &Application::getInfo() {
		return info;
	}
}


