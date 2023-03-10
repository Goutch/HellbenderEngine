//
// Created by user on 3/12/2022.
//

#include "SimCamera2DSystem.h"

SimCamera2DSystem::SimCamera2DSystem(Scene *scene) : System(scene) {
	scene->onUpdate.subscribe(this, &SimCamera2DSystem::update);
	scene->onAttach<SimCamera2D>().subscribe(this, &SimCamera2DSystem::onAttachSimCamera);
}

SimCamera2DSystem::~SimCamera2DSystem() {
	scene->onUpdate.unsubscribe(this);
	scene->onAttach<SimCamera2D>().unsubscribe(this);
}

void SimCamera2DSystem::onAttachSimCamera(Entity entity) {
	if (!entity.has<Camera2D>())
		entity.attach<Camera2D>();
}

void SimCamera2DSystem::update(float delta) {
	for (auto [entity, camera, sim_camera]: scene->group<Camera2D, SimCamera2D>()) {
		camera.setZoomRatio(sim_camera.zoom_ratio.toFloat());
	}
}
