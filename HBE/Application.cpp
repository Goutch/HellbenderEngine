#include "Application.h"
#include "core/graphics/Graphics.h"
#include "core/input/Input.h"
#include "GLFW/glfw3.h"
#include "core/utility/Clock.h"
#include "Configs.h"
#include "core/threading/JobManager.h"
#include <core/graphics/Window.h>
#include <core/graphics/Renderer.h>
#include <core/resource/Resources.h>
#include "core/utility/Profiler.h"
#include "core/scene/Scene.h"
#include "core/resource/RenderTarget.h"
#include "core/audio/Audio.h"
#include <chrono>
#include <thread>

namespace HBE {
	ApplicationInfo Application::info = {};
	Window *Application::window = nullptr;
	Clock *Application::time = nullptr;
	int Application::fps_counter = 0;
	float Application::fps_timer = 0;
	Event<> Application::onInit = Event<>();
	Event<Scene *> Application::onSceneChange = Event<Scene *>();
	Event<> Application::onWindowClosed = Event<>();
	Event<> Application::onQuit = Event<>();

	Event<float> Application::onUpdate;
	Event<> Application::onDraw;
	Event<> Application::onRender;
	Event<> Application::onPresent;

	void Application::init(const ApplicationInfo &info) {
		Application::info = info;
		Graphics::init();
		Audio::init();
		Input::init();
		window = Graphics::getWindow();
		onInit.invoke();
	}

	void Application::run() {

		time = new Clock();
		Clock update_clock = Clock();
		float delta_t = 0.0f;
		while (!window->shouldClose()) {
			window->swapBuffers();
			Input::pollEvents();
			HB_PROFILE_BEGIN("TOTAL_FRAME");
			JobManager::updateJobsStatus();
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
			Log::debug("fps:" + std::to_string(fps_counter));
			fps_counter = 0;
			fps_timer = 0;
		}
	}

	const ApplicationInfo &Application::getInfo() {
		return info;
	}
}


