#pragma once

#include "HBE.h"

#include "Pathfinder/Systems/AgentSystem.h"
#include "Pathfinder/Systems/PlayerInputSystem.h"
#include "Pathfinder/Systems/SimTransformSystem.h"
#include "Pathfinder/Systems/SimCamera2DSystem.h"
#include "Map.h"

class Pathfinder {
private:
	Entity camera;
	Map *map;
	Scene* scene;
public:
	void onUpdate(float delta) {

		float wheel = Input::getMouseWheelInput();
		SimCamera2D &cam = camera.get<SimCamera2D>();

		cam.zoom_ratio -= cam.zoom_ratio * f32(wheel) * f32(delta) * 100;
	}

	Pathfinder(Scene& scene) {
		map = new Map(100, 100);
		scene.onDraw.subscribe(map->navmesh, &Navmesh::draw);
		scene.addSystem(new PlayerInputSystem(*map, &scene));
		scene.addSystem(new AgentSystem(&scene));
		scene.addSystem(new SimCamera2DSystem(&scene));
		scene.addSystem(new SimTransformSystem(&scene));


		Entity e = scene.createEntity3D();
		Agent agent = e.attach<Agent>();
		e.attach<SimTransform>();

		camera = scene.createEntity3D();
		SimCamera2D &simCameraComponent = camera.attach<SimCamera2D>();
		Camera2D &cameraComponent = camera.attach<Camera2D>();
		camera.attach<SimTransform>();
		simCameraComponent.zoom_ratio = 10;
		cameraComponent.setZoomRatio(10);
		scene.setCameraEntity(camera);
		scene.onUpdate.subscribe(this, &Pathfinder::onUpdate);
		this->scene= &scene;
	}

	~Pathfinder() {
		scene->onDraw.unsubscribe(map->navmesh);
		delete map;
	}
};