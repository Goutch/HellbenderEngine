#include "Scene.h"

namespace HBE {
	Entity Scene::createEntity() {
		return Entity(registry.create(), &registry);
	}

	Entity Scene::getCameraEntity() {
		return main_camera_entity;
	}

	void Scene::destroyEntity(Entity entity) {
		registry.destroy(entity.handle);
	}

	void Scene::setCameraEntity(Entity camera) {
		main_camera_entity = camera;
	}
}
