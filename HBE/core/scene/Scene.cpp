#include "core/scene/systems/ModelRendererSystem.h"
#include "Scene.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Resources.h"
#include "core/resource/RenderTarget.h"
#include "Entity.h"
#include "list"

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

	bool Entity::has(component_type_id id) {
		return scene->has(handle, id);
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
		SceneNode *node = node_map[entity.getHandle()];
		node_map.erase(entity.getHandle());
		if (node->has_parent) {
			node->parent->children.remove(*node);
		} else {
			root_nodes.remove(*node);
		}
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
		node_map.emplace(e.getHandle(), &root_nodes.back());
		return e;
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

	bool Scene::has(entity_handle handle, size_t component_hash) {
		return registry.has(handle, component_hash);
	}

	void Scene::setParent(Entity entity, Entity parent) {
		SceneNode *node = node_map[entity.getHandle()];
		std::list<SceneNode> source = node->has_parent ? node->parent->children : root_nodes;
		std::list<SceneNode> destination = parent.valid() ? node_map[parent.getHandle()]->children : root_nodes;
		auto iterator = std::find(source.begin(), source.end(), *node);
		destination.splice(destination.end(), source, iterator, std::next(iterator));
		node->has_parent = parent.valid();
		if (node->has_parent)
			node->parent = node_map[parent.getHandle()];
		else
			node->parent = nullptr;

		if (entity.has<Transform>())
			entity.get<Transform>().is_dirty = true;
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
		const component_type_id id = registry.getComponentTypeID<Camera>();
		if (main_camera_entity.has(id)) {
			main_camera_entity.get<Camera>().calculateProjection();
		}
		if (main_camera_entity.has<Camera2D>()) {
			main_camera_entity.get<Camera2D>().calculateProjection();
		}
	}

	std::list<SceneNode> Scene::getSceneNodes() {
		return root_nodes;
	}
}