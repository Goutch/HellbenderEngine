#include "Scene.h"
#include "core/serialization/Serializer.h"
#include "core/serialization/Deserializer.h"
#include "core/utility/Log.h"
#include <algorithm>

namespace HBE {
	Scene::Scene() {

	}

	Scene::Scene(std::string path) {
		//todo:implement scene loading
	}

	void Scene::init() {
		on_init.invoke(this);
		for (Entity *e:scene_tree) {
			e->init();
		}
	}

	void Scene::update(float delta) {
		on_update.invoke(this, delta);
		for (Component *c:update_listeners) {
			c->onUpdate(delta);
		}
	}

	void Scene::draw() {
		on_draw.invoke(this);
		for (Component *c:draw_listeners) {
			c->onDraw();
		}
	}

	void Scene::render() {
		on_render.invoke(this);
	}

	void Scene::terminate() {
		on_terminate.invoke(this);
		for (Entity *e:scene_tree) {
			delete e;
		}
		scene_tree.clear();
	}


	Entity *Scene::instantiate(std::string name, Entity *parent) {
		Entity *e = new Entity(name, parent);
		scene_tree.emplace_back(e);
		e->init();
		return e;
	}

	Entity *Scene::instantiate(Entity *parent) {
		Entity *e = new Entity(parent);
		scene_tree.emplace_back(e);
		e->init();
		return e;
	}


	void Scene::destroy(Entity *e) {
		Log::debug("destroy " + e->getName());
		auto it = std::find(scene_tree.begin(), scene_tree.end(), e);
		if (it != scene_tree.end()) {
			scene_tree.erase(it);
		}
		e->destroy();
		delete e;
	}

	Scene::~Scene() {
		terminate();
	}

	void Scene::subscribeDraw(Component *component) {
		draw_listeners.emplace(component);
	}

	void Scene::unsubscribeDraw(Component *component) {
		draw_listeners.erase(component);
	}

	void Scene::subscribeUpdate(Component *component) {
		update_listeners.emplace(component);
	}

	void Scene::unsubscribeUpdate(Component *component) {
		update_listeners.erase(component);
	}

	void Scene::serialize(Serializer *serializer) const {
		serializer->begin("Scene");
		serializer->beginArray("entities");
		for (auto e: scene_tree) {
			if (e->transform->getParent() == nullptr)
				e->serialize(serializer);
		}
		serializer->endArray();
		serializer->end();
	}

	void Scene::deserialize(Deserializer *deserializer) {

	}

	const std::vector<Entity *> &Scene::getSceneTree() {
		return scene_tree;
	}


}










