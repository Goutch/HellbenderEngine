
#include "PlayerInputSystem.h"
#include "SimTransform.h"
#include "AgentSystem.h"

PlayerInputSystem::PlayerInputSystem(Scene *scene) : System(scene) {
	scene->onUpdate.subscribe(this, &PlayerInputSystem::update);
}

PlayerInputSystem::~PlayerInputSystem() {
	scene->onUpdate.unsubscribe(this);
}

void PlayerInputSystem::update(float delta) {

	fixed3<f32> target = fixed3<f32>(0, 0, 0);
	Camera2D &camera = scene->getCameraEntity()->get<Camera2D>();
	const SimTransform &camera_transform = scene->getCameraEntity()->get<SimTransform>();
	f32 zoom_ratio = f32(camera.zoom_ratio);
	if (Input::getKey(KEY::MOUSE_BUTTON_RIGHT)) {
		double x, y;
		Input::getMousePosition(x, y);
		uint32_t width, height;
		Graphics::getWindow()->getSize(width, height);

		target = fixed3<f32>(f32(x) - (f32(static_cast<int32_t>(width)) * f32::HALF),
							 (f32(static_cast<int32_t>(height)) - f32(y)) - (f32(static_cast<int32_t>(height)) * f32::HALF),
							 0);
		target /= fixed3<f32>(f32(static_cast<int32_t>(width)) / (zoom_ratio * f32(camera.aspectRatio())),
							  f32(static_cast<int32_t>(height)) / zoom_ratio,
							  1);

		target += camera_transform.position;

		auto group = scene->group<Agent>();

		for (auto[entity, agent]:group) {
			agent.target_position = target;
		}
	}
}
