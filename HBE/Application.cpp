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

		while (!window->shouldClose() && current_scene != nullptr) {
			window->swapBuffers();
			Input::pollEvents();

			JobManager::updateJobsStatus();
			onUpdate.invoke(delta_t);
			current_scene->update(delta_t);

			Entity camera_entity = current_scene->getCameraEntity();

			if (camera_entity.valid()) {
				Camera &camera = camera_entity.get<Camera>();
				if (camera.active) {
					onDraw.invoke();
					current_scene->draw();
					Graphics::beginFrame();
					onRender.invoke();
					current_scene->render();
					Graphics::endFrame();
				}
			} else {
				Log::warning("No camera in current scene");
			}
			delta_t = update_clock.ns() / SECONDS_TO_NANOSECOND;
			update_clock.reset();
#ifdef DEBUG_MODE
			printFPS(delta_t);
#endif
		}
		onWindowClosed.invoke();
		onQuit.invoke();
		delete time;
		delete current_scene;
	}

	void Application::terminate() {
		Graphics::terminate();
	}

	void Application::quit() {
		window->requestClose();
	}

	float Application::getTime() {
		if (time) {
			return time->ms() / SECOND_TO_MILISECOND;
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


