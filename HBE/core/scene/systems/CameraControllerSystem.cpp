

#include "CameraControllerSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"
#include "core/utility/Profiler.h"

namespace HBE {
	CameraControllerSystem::CameraControllerSystem(Scene *scene) : System(scene) {
		scene->onUpdate.subscribe(this, &CameraControllerSystem::update);
	}

	void CameraControllerSystem::update(float delta_t) {
		Profiler::begin("CameraControllerUpdate");
		Profiler::begin("CameraControllerUpdateGroup");
		auto group = scene->group<Transform, Camera, CameraController>();
		Profiler::end();
		for (auto[handle, transform, camera, controller]:group) {
			float max_pitch_radian = glm::radians(controller.max_pitch);
			uint32_t w, h;
			camera.render_target->getResolution(w, h);
			float width = static_cast<float>(w), height = static_cast<float>(h);
			double x, y;
			Input::getMousePosition(x, y);
			Input::setCursorPosition(w / 2.0, h / 2.0f);

			float fov = glm::radians(camera.fov);
			vec2 change;
			change.x = (((width / 2.0f) - x) / width) * fov;
			change.y = (((height / 2.0f) - y) / height) * (fov * camera.aspectRatio());

			//go to pitch 0
			transform.rotate(vec3(-controller.current_pitch, 0, 0));
			//rotate on y axis
			transform.rotate(vec3(0, change.x, 0));
			//go back to current pitch
			controller.current_pitch += change.y;
			controller.current_pitch = glm::clamp(controller.current_pitch, -max_pitch_radian, max_pitch_radian);

			transform.rotate(vec3(controller.current_pitch, 0, 0));

			float boost = 1.0f;
			vec3 translation = vec3(0);
			if (Input::getKey(KEY::S)) {
				translation.z += 1;
			}
			if (Input::getKey(KEY::W)) {
				translation.z = -1;
			}
			if (Input::getKey(KEY::D)) {
				translation.x += 1;
			}
			if (Input::getKey(KEY::A)) {
				translation.x = -1;
			}
			if (Input::getKey(KEY::SPACE)) {
				translation.y += 1;
			}
			if (Input::getKey(KEY::LEFT_CONTROL)) {
				translation.y = -1;
			}
			if (Input::getKey(KEY::LEFT_SHIFT)) {
				boost = 10.0f;
			}

			transform.translate(translation * delta_t * controller.speed * boost);
		}
		Profiler::end();

	}

	CameraControllerSystem::~CameraControllerSystem() {
		scene->onUpdate.unsubscribe(this);
	}
}

