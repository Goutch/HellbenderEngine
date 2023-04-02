#pragma once

#include "Scene.h"
#include "Entity.h"
#include "vector"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/CameraControllerSystem.h"
#include "core/scene/systems/ModelRendererSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"

namespace HBE {


	Entity Scene::getCameraEntity() {
		return main_camera_entity;
	}

	void Scene::destroyEntity(entity_handle entity) {
		SceneNode *node = node_map[entity];

		if (node->has_parent) {
			node->parent->children.remove(*node);
		} else {
			root_nodes.remove(*node);
		}
		node_map.erase(entity);
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> signature = registry.getSignature(entity);

		for (auto e: detach_events) {
			if (signature.test(e.first)) {
				e.second.invoke(Entity(entity, this));
			}
		}
		registry.destroy(entity);
	}

	void Scene::setCameraEntity(Entity camera) {
		main_camera_entity = camera;
	}

	void Scene::update(float delta_t) {
		if (is_active) {
			onUpdate.invoke(delta_t);
		}
	}

	void Scene::draw() {
		if (is_active) {
			onDraw.invoke(&render_graph);
		}
	}

	void Scene::render() {
		if (is_active) {
			onRender.invoke(&render_graph);
		}
	}

	Scene::~Scene() {
		Graphics::getDefaultRenderTarget()->onResolutionChange.unsubscribe(this);
		for (auto event: detach_events) {
			std::vector<entity_handle> entities;
			registry.group(entities, event.first);
			for (unsigned int entitie: entities) {
				event.second.invoke(Entity(entitie, this));
			}
		}

		for (int i = 0; i < systems.size(); ++i) {
			delete systems[i];
		}
		Application::onDraw.unsubscribe(this);
		Application::onRender.unsubscribe(this);
		Application::onUpdate.unsubscribe(this);
	}

	bool Scene::valid(entity_handle handle) {

		return registry.valid(handle);
	}

	Entity Scene::createEntity() {
		Entity e = Entity(registry.create(), this);
		root_nodes.push_back(SceneNode{e});
		node_map.emplace(e.getHandle(), &root_nodes.back());
		return e;
	}

	bool Scene::has(entity_handle handle, size_t signature_bit) {
		return registry.has(handle, signature_bit);
	}

	Entity Scene::getParent(Entity entity) {
		SceneNode *node = node_map[entity.getHandle()];
		if (node->has_parent) {
			return node->parent->entity;
		}
		return {};
	}

	const std::list<SceneNode> &Scene::getChildren(Entity entity) {
		return node_map[entity.getHandle()]->children;
	}

	void Scene::printNode(SceneNode &node, int level) {
		std::string indent = "";
		for (int i = 0; i < level; ++i) {
			indent += '\t';
		}
		Log::message(indent + std::to_string(node.entity.getHandle()));
		for (auto &child: node.children) {
			printNode(child, level + 1);
		}
	}

	void Scene::printSceneHierarchy() {
		for (auto &node: root_nodes) {
			printNode(node, 0);
		}
	}

	SceneNode *Scene::getNode(Entity entity) {
		return &*node_map[entity.getHandle()];
	}

	std::list<SceneNode> Scene::getSceneNodes() {
		return root_nodes;
	}

	void Scene::setActive(bool active) {
		if (active != is_active) {
			if (active)
				onSceneActivate.invoke(this);
			else
				onSceneDeactivate.invoke(this);
		}
		is_active = active;
	}

	bool Scene::isActive() {
		return is_active;
	}

	void Scene::onFrameChange(uint32_t frame) {
		render_graph.clear();
	}

	bool Entity::valid() {
		return scene != nullptr && scene->valid(handle);
	}

	entity_handle Entity::getHandle() const {
		return handle;
	}

	Entity::Entity(const Entity &other) {
		this->handle = other.handle;
		this->scene = other.scene;
	}

	Entity::Entity(entity_handle handle, Scene *scene) {
		this->handle = handle;
		this->scene = scene;
	}

	bool Entity::has(size_t sigature_bit) {
		return scene->has(handle, sigature_bit);
	}

	Scene *Entity::getScene() {
		return scene;
	}

	Entity Entity::getParent() const {
		return scene->getParent(*this);
	}

	void Entity::destroy() {
		scene->destroyEntity(handle);
	}

	void Scene::addSystem(System *system) {
		systems.push_back(system);
	}

	Scene::Scene() {
		onAttach<Transform>().subscribe(this, &Scene::onAttachTransform);

		systems.reserve(4);
		systems.push_back(new CameraSystem(this));
		systems.push_back(new CameraControllerSystem(this));
		systems.push_back(new ModelRendererSystem(this));
		systems.push_back(new MeshRendererSystem(this));

		Application::onDraw.subscribe(this, &Scene::draw);
		Application::onRender.subscribe(this, &Scene::render);
		Application::onUpdate.subscribe(this, &Scene::update);
		Graphics::onFrameChange.subscribe(this, &Scene::onFrameChange);

	}

	Entity Scene::createEntity3D() {
		Entity e(registry.create(), this);
		root_nodes.emplace_back(SceneNode{e, false});
		node_map.emplace(e.getHandle(), &root_nodes.back());
		attach<Transform>(e.getHandle());
		return e;
	}

	Entity Scene::createEntity2D() {
		Entity e(registry.create(), this);
		root_nodes.emplace_back(SceneNode{e, false});
		node_map.emplace(e.getHandle(), &root_nodes.back());
		attach<Transform2D>(e.getHandle());
		return e;
	}

	void Scene::setParent(Entity entity, Entity parent) {
		SceneNode *node = node_map[entity.getHandle()];
		std::list<SceneNode> &source = node->has_parent ? node->parent->children : root_nodes;
		std::list<SceneNode> &destination = parent.valid() ? node_map[parent.getHandle()]->children : root_nodes;
		auto iterator = std::find(source.begin(), source.end(), *node);
		destination.splice(destination.end(), source, iterator, std::next(iterator));
		node->has_parent = parent.valid();
		if (node->has_parent)
			node->parent = node_map[parent.getHandle()];
		else
			node->parent = nullptr;

		if (entity.has<Transform>()) {
			Transform &transform = entity.get<Transform>();
			transform.is_dirty = true;
			transform.parent = &parent.get<Transform>();
			setChildrenDirty(node);
		}
	}

	void Scene::onAttachTransform(Entity entity) {
		Transform &transform = entity.get<Transform>();
	}

	Texture *Scene::getMainCameraTexture() {
		if (main_camera_entity.valid() && (main_camera_entity.has<Camera>() || main_camera_entity.has<Camera2D>() || main_camera_entity.has<PixelCamera>())) {

			if (main_camera_entity.valid() && (main_camera_entity.has<Camera>())) {
				return main_camera_entity.get<Camera>().getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<Camera2D>())) {
				return main_camera_entity.get<Camera2D>().getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<PixelCamera>())) {
				return main_camera_entity.get<PixelCamera>().getRenderTarget()->getFramebufferTexture(Graphics::getCurrentFrame());
			}
		}
		return nullptr;
	}

	void Scene::setChildrenDirty(SceneNode *node) {
		for (SceneNode &child: node->children) {
			if (child.entity.has<Transform>()) {
				child.entity.get<Transform>().is_dirty = true;
				setChildrenDirty(&child);
			}
		}
	}
}