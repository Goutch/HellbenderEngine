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
	Scene *Application::scene = nullptr;
	Window *Application::window = nullptr;
	Clock *Application::time = nullptr;
	int Application::fps_counter = 0;
	float Application::fps_timer = 0;
	Event<> Application::onInit = Event<>();
	Event<float> Application::onUpdate = Event<float>();
	Event<> Application::onDraw = Event<>();
	Event<Scene *> Application::onSceneChange = Event<Scene *>();
	Event<> Application::onRender = Event<>();
	Event<> Application::onRegisterComponents = Event<>();
	Event<> Application::onWindowClosed = Event<>();
	Event<> Application::onQuit = Event<>();

	void Application::registerComponents() {
		onRegisterComponents.invoke();
	}

	Scene *Application::setScene(std::string path) {
		//todo:scene loading
		if (scene != nullptr) {
			delete scene;
		}
		scene = new Scene();
		onSceneChange.invoke(scene);
		return scene;
	}

	void Application::init() {
		Graphics::init();
		window = Graphics::getWindow();
		registerComponents();
		Input::init();
		if (!scene)
			setScene("");
		onInit.invoke();
	}

	void Application::run() {

		time = new Clock();
		Clock update_clock = Clock();
		float delta_t = 0.0f;

		while (!window->shouldClose() && scene != nullptr) {
			window->swapBuffers();
			Input::pollEvents();

			JobManager::updateJobsStatus();
			onUpdate.invoke(delta_t);

			Entity camera_entity = scene->getCameraEntity();

			if (camera_entity.valid()) {
				Camera &camera = camera_entity.get<Camera>();
				Transform transform = camera_entity.get<Transform>();
				onDraw.invoke();
				Graphics::beginFrame();
				Graphics::render(camera.render_target,
								 camera.projection,
								 glm::inverse(transform.matrix));
				Graphics::endFrame();
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
		delete scene;
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


