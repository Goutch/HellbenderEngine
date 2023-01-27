#include "core/scene/systems/ModelRendererSystem.h"
#include "Scene.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Resources.h"
#include "core/resource/RenderTarget.h"

namespace HBE {

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


	Scene::Scene() {
		systems.push_back(new MeshRendererSystem(this));
		systems.push_back(new ModelRendererSystem(this));
		systems.push_back(new CameraSystem(this));
		systems.push_back(new CameraControllerSystem(this));
		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(this, &Scene::calculateCameraProjection);
		onAttach<Transform>().subscribe(this, &Scene::onAttachTransform);
	}

	Entity Scene::getCameraEntity() {
		return main_camera_entity;
	}

	void Scene::destroyEntity(Entity entity) {
		removeFromTree(entity);
		registry.destroy(entity.getHandle());
	}

	void Scene::setCameraEntity(Entity camera) {
		main_camera_entity = camera;
		calculateCameraProjection(Graphics::getDefaultRenderTarget());
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
		Graphics::getDefaultRenderTarget()->onResolutionChange.unsubscribe(this);
		for (auto event: detach_events) {
			std::vector<entity_handle> entities;
			registry.group(entities, event.first);
			for (unsigned int entitie: entities) {
				event.second.invoke(Entity(entitie, this));
			}
		}
		for (System *system: systems) {
			delete system;
		}
	}

	void Scene::addSystem(System *system) {
		systems.emplace_back(system);
	}

	bool Scene::valid(entity_handle handle) {

		return registry.valid(handle);
	}

	Entity Scene::createEntity() {
		Entity e = Entity(registry.create(), this);
		root_nodes.push_back(SceneNode{e});
		node_map.emplace(e.getHandle(), root_nodes.end());
		return e;
	}


	Entity Scene::createEntity3D() {
		Entity e(registry.create(), this);
		root_nodes.emplace_back(SceneNode{e});
		node_map.emplace(e.getHandle(), root_nodes.end());
		attach<Transform>(e.getHandle());
		return e;
	}

	Entity Scene::createEntity2D() {
		Entity e(registry.create(), this);
		root_nodes.emplace_back(SceneNode{e});
		node_map.emplace(e.getHandle(), root_nodes.end());
		attach<Transform2D>(e.getHandle());
		return e;
	}

	bool Scene::has(entity_handle handle, size_t component_hash) {
		return registry.has(handle, component_hash);
	}

	void Scene::removeFromTree(Entity entity) {
		const std::list<SceneNode>::iterator node = node_map[entity.getHandle()];
		if (node->has_parent) {
			node->parent->has_parent = false;
			node->parent->children.erase(node);
		} else {
			root_nodes.erase(node);
		}
		node_map.erase(node->entity.getHandle());
	}

	void Scene::setParent(Entity entity, Entity parent) {
		removeFromTree(entity);
		if (parent.valid()) {
			std::list<SceneNode>::iterator parent_node = node_map[parent.getHandle()];
			SceneNode node = SceneNode{entity, true, parent_node};
			parent_node->children.push_back(node);
			node_map.emplace(entity.getHandle(), parent_node->children.end());
		} else {
			SceneNode node = SceneNode{entity, false};
			root_nodes.push_back(node);
			node_map.emplace(entity.getHandle(), root_nodes.end());
		}
	}

	Entity Scene::getParent(Entity entity) {
		const std::list<SceneNode>::iterator node = node_map[entity.getHandle()];
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

	void Scene::print() {
		for (auto &node: root_nodes) {
			printNode(node, 0);
		}
	}

	void Scene::onAttachTransform(Entity entity) {
		Transform &transform = entity.get<Transform>();
		transform.entity = entity;
	}

	SceneNode *Scene::getNode(Entity entity) {
		return &*node_map[entity.getHandle()];
	}

	void Scene::calculateCameraProjection(RenderTarget *renderTarget) {
		if (!main_camera_entity.valid()) return;

		if (main_camera_entity.has<Camera>()) {
			main_camera_entity.get<Camera>().calculateProjection();
		}
		if (main_camera_entity.has<Camera2D>()) {
			main_camera_entity.get<Camera2D>().calculateProjection();
		}

	}
}