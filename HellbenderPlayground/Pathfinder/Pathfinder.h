#pragma once

#include "HBE.h"

#include "AgentSystem.h"
#include "PlayerInputSystem.h"
#include "SimTransform.h"

class Pathfinder {
private:
	Entity camera;
public:
	void onUpdate(float delta) {
		float wheel = 0.0f;
		Input::getMouseWheelInput(wheel);
		Camera2D &cam = camera.get<Camera2D>();
		cam.zoom_ratio -= cam.zoom_ratio * wheel * delta * 100;
		cam.calculateProjection();
	}

	Pathfinder() {
		Application::setScene(new Scene(), true);

		Scene &scene = *Application::getScene();
		scene.addSystem(new PlayerInputSystem(&scene));
		scene.addSystem(new AgentSystem(&scene));
		scene.addSystem(new SimTransformSystem(&scene));


		Entity e = scene.createEntity();
		Agent agent = e.attach<Agent>();
		e.attach<SimTransform>();

		camera = scene.createEntity();
		Camera2D &cameraComponent = camera.attach<Camera2D>();
		camera.attach<SimTransform>();
		cameraComponent.zoom_ratio = 10;
		cameraComponent.calculateProjection();
		scene.setCameraEntity(camera);
		scene.onUpdate.subscribe(this, &Pathfinder::onUpdate);
	}
};