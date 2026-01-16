#include "CameraControllerSystem.h"
#include "core/input/Input.h"
#include "core/utility/Profiler.h"
#include "Application.h"
#include "core/scene/components/CameraController.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/Node.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/Transform.h"

namespace HBE {
	CameraControllerSystem::CameraControllerSystem(Scene *scene) : System(scene) {
		scene->onUpdate.subscribe(this, &CameraControllerSystem::update);
		Event<Entity> &e = scene->onDetach<CameraController>();
		e.subscribe(this, &CameraControllerSystem::onDetach);
		e.subscribe(this, &CameraControllerSystem::onAttach);
	}

	void CameraControllerSystem::update(float delta_t) {
		HB_PROFILE_BEGIN("CameraControllerUpdate");
		HB_PROFILE_BEGIN("CameraControllerUpdateGroup");
		auto group = scene->group<Node, Transform, Camera, CameraController>();
		HB_PROFILE_END("CameraControllerUpdateGroup");
		for (auto [handle, node, transform, camera, controller]: group) {
			if (!node.isActiveInHierarchy()) {
				continue;
			}
			float max_pitch_radian = glm::radians(controller.max_pitch);

			vec2i resolution = camera.getRenderTarget()->getResolution();
			float width = static_cast<float>(resolution.x);
			float height = static_cast<float>(resolution.y);

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

			if (controller.invert_y)
				controller.current_pitch += mouse_delta.y;
			else
				controller.current_pitch -= mouse_delta.y;

			controller.current_pitch = glm::clamp(controller.current_pitch, -max_pitch_radian, max_pitch_radian);

			transform.rotate(vec3(controller.current_pitch, 0, 0));

			vec3 translation = vec3(0);
			if (Input::getKey(KEY_S)) {
				translation.z += 1;
			}
			if (Input::getKey(KEY_W)) {
				translation.z = -1;
			}
			if (Input::getKey(KEY_D)) {
				translation.x += 1;
			}
			if (Input::getKey(KEY_A)) {
				translation.x = -1;
			}
			if (Input::getKey(KEY_SPACE)) {
				translation.y += 1;
			}
			if (Input::getKey(KEY_LEFT_CONTROL)) {
				translation.y = -1;
			}
			if (Input::getKey(KEY_LEFT_SHIFT)) {
				controller.acceleration_time += delta_t * controller.acceleration;
				//controller.acceleration_time = glm::min(controller.acceleration_time, 1.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			} else {
				controller.acceleration_time -= delta_t * controller.acceleration;
				controller.acceleration_time = glm::max(controller.acceleration_time, 0.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			}

			transform.translate(translation * delta_t * controller.current_speed);
		}
		auto group2D = scene->group<Transform, Camera2D, CameraController>();
		for (auto [handle, transform, camera, controller]: group2D) {
			vec3 translation = vec3(0);
			if (Input::getKey(KEY_EQUAL)) {
				camera.setZoomRatio(camera.getZoomRatio() + (delta_t));
			}
			if (Input::getKey(KEY_MINUS)) {
				camera.setZoomRatio(camera.getZoomRatio() - (delta_t));
			}
			if (Input::getKey(KEY_D)) {
				translation.x += 1;
			}
			if (Input::getKey(KEY_A)) {
				translation.x = -1;
			}
			if (Input::getKey(KEY_W)) {
				translation.y += 1;
			}
			if (Input::getKey(KEY_S)) {
				translation.y = -1;
			}
			if (Input::getKey(KEY_LEFT_SHIFT)) {
				controller.acceleration_time += delta_t * controller.acceleration;
				controller.acceleration_time = glm::min(controller.acceleration_time, 1.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			} else {
				controller.acceleration_time -= delta_t * controller.acceleration;
				controller.acceleration_time = glm::max(controller.acceleration_time, 0.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			}

			transform.translate(translation * delta_t * controller.current_speed);
		}
		auto group_pixel = scene->group<Transform, PixelCamera, CameraController>();
		for (auto [handle, transform, camera, controller]: group_pixel) {
			float boost = 1.0f;
			vec3 translation = vec3(0);
			if (Input::getKey(KEY_LEFT_SHIFT)) {
				boost = 10.0f;
			}
			if (Input::getKey(KEY_D)) {
				translation.x += 1;
			}
			if (Input::getKey(KEY_A)) {
				translation.x = -1;
			}
			if (Input::getKey(KEY_W)) {
				translation.y += 1;
			}
			if (Input::getKey(KEY_S)) {
				translation.y = -1;
			}
			if (Input::getKey(KEY_LEFT_SHIFT)) {
				controller.acceleration_time += delta_t * controller.acceleration;
				controller.acceleration_time = glm::min(controller.acceleration_time, 1.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			} else {
				controller.acceleration_time -= delta_t * controller.acceleration;
				controller.acceleration_time = glm::max(controller.acceleration_time, 0.0f);
				controller.current_speed = std::lerp(controller.base_speed, controller.max_speed, controller.acceleration_time);
			}

			transform.translate(translation * delta_t * controller.current_speed);
		}
		HB_PROFILE_END("CameraControllerUpdate");
	}

	CameraControllerSystem::~CameraControllerSystem() {
		scene->onUpdate.unsubscribe(this);
	}

	void CameraControllerSystem::onAttach(Entity entity) {
		Input::setCursorVisible(false);
	}

	void CameraControllerSystem::onDetach(Entity entity) {
		Input::setCursorVisible(true);
		CameraController *c = entity.get<CameraController>();
		Transform *t = entity.get<Transform>();
		t->rotate(vec3(-c->current_pitch, 0, 0));
	}
}
