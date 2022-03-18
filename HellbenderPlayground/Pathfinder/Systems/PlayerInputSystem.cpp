
#include "PlayerInputSystem.h"
#include "SimTransformSystem.h"
#include "AgentSystem.h"
#include "SimCamera2DSystem.h"


PlayerInputSystem::PlayerInputSystem(Map &map, Scene *scene) : map(map), System(scene) {
	scene->onUpdate.subscribe(this, &PlayerInputSystem::update);
}


PlayerInputSystem::~PlayerInputSystem() {
	scene->onUpdate.unsubscribe(this);
}


void PlayerInputSystem::update(float delta) {

	f32 step = f32(delta);
	fixed3<f32> click_position = fixed3<f32>(0, 0, 0);
	SimCamera2D &sim_camera = scene->getCameraEntity()->get<SimCamera2D>();
	Camera2D &camera = scene->getCameraEntity()->get<Camera2D>();
	SimTransform &camera_transform = scene->getCameraEntity()->get<SimTransform>();
	f32 zoom_ratio = f32(camera.zoom_ratio);
	bool right_button = Input::getKey(KEY::MOUSE_BUTTON_RIGHT);
	bool left_button = Input::getKey(KEY::MOUSE_BUTTON_LEFT);
	if (right_button || left_button) {
		double x, y;
		Input::getMousePosition(x, y);
		uint32_t width, height;
		Graphics::getWindow()->getSize(width, height);

		click_position = fixed3<f32>(f32(x) - (f32(static_cast<int32_t>(width)) * f32::HALF),
									 (f32(static_cast<int32_t>(height)) - f32(y)) - (f32(static_cast<int32_t>(height)) * f32::HALF),
									 0);
		click_position /= fixed3<f32>(f32(static_cast<int32_t>(width)) / (zoom_ratio * f32(camera.aspectRatio())),
									  f32(static_cast<int32_t>(height)) / zoom_ratio,
									  1);

		click_position += camera_transform.position;
		if (right_button) {
			auto group = scene->group<Agent>();

			for (auto[entity, agent]:group) {
				agent.target_position = click_position;
			}
		}

		if (left_button) {
			map.set(click_position.x.round(), click_position.y.round(), true);
		}
	}
	if (Input::getKey(KEY::W)) {
		camera_transform.position.y += sim_camera.zoom_ratio * step;
	}
	if (Input::getKey(KEY::A)) {
		camera_transform.position.x -= sim_camera.zoom_ratio * step;
	}
	if (Input::getKey(KEY::D)) {
		camera_transform.position.x += sim_camera.zoom_ratio * step;
	}
	if (Input::getKey(KEY::S)) {
		camera_transform.position.y -= sim_camera.zoom_ratio * step;
	}
}



