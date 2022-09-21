

#include "CameraControllerSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"
#include "core/utility/Profiler.h"
#include "core/resource/RenderTarget.h"
#include "core/graphics/Graphics.h"
#include "core/input/Input.h"
#include "Application.h"

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

			vec2i resolution = camera.render_target->getResolution();
			float width = static_cast<float>(resolution.x), height = static_cast<float>(resolution.y);

			vec2 mouse_pos = Input::getMousePosition();
			Input::setCursorPosition(resolution.x / 2.0, resolution.y / 2.0f);

			float fov = glm::radians(camera.fov);
			vec2 mouse_delta;
			mouse_delta.x = (((width / 2.0f) - mouse_pos.x) / width) * fov;
			mouse_delta.y = (((height / 2.0f) - mouse_pos.y) / height) * (fov * camera.aspectRatio());

			//go to pitch 0
			transform.rotate(vec3(-controller.current_pitch, 0, 0));
			//rotate on y axis
			transform.rotate(vec3(0, mouse_delta.x, 0));
			//go back to current pitch
			controller.current_pitch += mouse_delta.y;
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

