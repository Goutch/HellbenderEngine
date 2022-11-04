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

namespace HBE {
	Scene *Application::current_scene = nullptr;
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

	void Application::setScene(Scene *scene, bool delete_previous_scene) {
		if (delete_previous_scene && scene != nullptr) {
			delete current_scene;
			current_scene = nullptr;
		}
		Application::current_scene = scene;
		if (scene != nullptr) {
			onSceneChange.invoke(scene);
		}

	}

	Scene *Application::getScene() {
		if (current_scene == nullptr) {
			setScene(new Scene(), false);
		}
		return current_scene;
	}

	void Application::init() {
		Graphics::init();
		window = Graphics::getWindow();
		Input::init();
		if (!current_scene)
			setScene(new Scene());
		onInit.invoke();
	}

	void Application::run() {

		time = new Clock();
		Clock update_clock = Clock();
		float delta_t = 0.0f;
		if (current_scene == nullptr) {
			setScene(new Scene());
		}
		while (!window->shouldClose() && current_scene != nullptr) {

			window->swapBuffers();
			Graphics::beginFrame();
			Input::pollEvents();
			Profiler::begin("TOTAL_FRAME");
			JobManager::updateJobsStatus();
			Profiler::begin("UPDATE");

			onUpdate.invoke(delta_t);
			current_scene->update(delta_t);
			Profiler::end();
			if (current_scene != nullptr) {
				Entity camera_entity = current_scene->getCameraEntity();

				if (camera_entity.valid()) {

					if (!window->isMinimized() &&
						(camera_entity.has<Camera>() ||
						 camera_entity.has<Camera2D>())) {

						Profiler::begin("DRAW");
						onDraw.invoke();
						current_scene->draw();
						Profiler::end();


						Profiler::begin("RENDER");


						onRender.invoke();
						current_scene->render();


						Profiler::end();
					}
				} else {
					//Log::warning("No camera in current scene");
				}
			} else {
				//Log::warning("No scene set");
			}
			Graphics::endFrame();
			Profiler::end();
			delta_t = update_clock.ns() * NANOSECONDS_TO_SECONDS;
			update_clock.reset();
			printFPS(delta_t);
		}

		onWindowClosed.invoke();
		onQuit.invoke();
		delete time;
	}

	void Application::terminate() {
		delete current_scene;
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
}


