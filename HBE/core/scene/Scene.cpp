#include "Scene.h"
#include "core/graphics/Graphics.h"
#include "core/scene/systems/CameraControllerSystem.h"

namespace HBE {
	Scene::Scene() {
		systems.push_back(new MeshRendererSystem(this));
		systems.push_back(new CameraSystem(this));
		systems.push_back(new CameraControllerSystem(this));
	}

	Entity Scene::createEntity(const std::string &name) {
		Entity e(registry.create(), &registry);
		e.attach<Identity>(name);
		e.attach<Transform>();
		return e;
	}


	Entity Scene::createEntity() {
		Entity e(registry.create(), &registry);
		e.attach<Transform>();
		return e;
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

	Entity Scene::get(const std::string &entity_name) {
		return entities[entity_name];
	}

	void Scene::update(float delta_t) {
		onUpdate.invoke(delta_t);
	}

	void Scene::draw() {
		onDraw.invoke();
	}

	void Scene::render() {
		onRender.invoke();
	}

	Scene::~Scene() {
		for (System *system:systems) {
			delete system;
		}
	}


}
