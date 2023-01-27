#include "core/scene/systems/ModelRendererSystem.h"
#include "core/scene/systems/TextRendererSystem.h"
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
		systems.push_back(new TextRendererSystem(this));
		Graphics::getDefaultRenderTarget()->onResolutionChange.subscribe(this, &Scene::calculateCameraProjection);
	}

	Entity Scene::getCameraEntity() {
		return main_camera_entity;
	}

	void Scene::destroyEntity(Entity entity) {
		SceneNode *node = node_map[entity.getHandle()];
		removeFromTree(node);
		node_map.erase(entity.getHandle());
		registry.destroy(entity.getHandle());
	}

	void Scene::calculateCameraProjection(RenderTarget *renderTarget) {
		if (!main_camera_entity.valid()) return;
		if (main_camera_entity.has<Camera>()) {
			main_camera_entity.get<Camera>().calculateProjection();
		} else if (main_camera_entity.has<Camera2D>()) {
			main_camera_entity.get<Camera2D>().calculateProjection();
		}
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
		for (auto event:detach_events) {
			std::vector<entity_handle> entities;
			registry.group(entities, event.first);
			for (unsigned int entitie : entities) {
				event.second.invoke(Entity(entitie, this));
			}
		}
		for (System *system:systems) {
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
		node_map.emplace(e.getHandle(), &root_nodes.emplace_back(SceneNode{e}));
		return e;
	}

	Entity Scene::createEntity(const std::string &name) {
		Entity e = createEntity3D();
		Identity &identity = e.attach<Identity>();
		identity.name = name;
		node_map.emplace(e.getHandle(), &root_nodes.emplace_back(SceneNode{e}));
		return e;
	}

	Entity Scene::createEntity3D() {
		Entity e(registry.create(), this);
		attach<Transform>(e.getHandle());
		node_map.emplace(e.getHandle(), &root_nodes.emplace_back(SceneNode{e}));
		return e;
	}

	Entity Scene::createEntity2D() {
		Entity e(registry.create(), this);
		attach<Transform2D>(e.getHandle());
		node_map.emplace(e.getHandle(), &root_nodes.emplace_back(SceneNode{e}));
		return e;
	}

	bool Scene::has(entity_handle handle, size_t component_hash) {
		return registry.has(handle, component_hash);
	}

	void Scene::removeFromTree(SceneNode *node) {
		if (node->parent != nullptr) {
			SceneNode *parent_node = node->parent;
			for (int i = 0; i < parent_node->children.size(); ++i) {
				if (parent_node->children[i].entity.getHandle() == node->entity.getHandle()) {
					parent_node->children.erase(parent_node->children.begin() + i);
					break;
				}
			}
			node->parent = nullptr;
		} else {
			for (int i = 0; i < root_nodes.size(); ++i) {
				if (root_nodes[i].entity.getHandle() == node->entity.getHandle()) {
					root_nodes.erase(root_nodes.begin() + i);
					break;
				}
			}
		}
	}

	void Scene::setParent(Entity entity, Entity parent) {
		SceneNode *node = node_map[entity.getHandle()];

		removeFromTree(node);
		if (parent.valid()) {
			SceneNode *parent_node = node_map[parent.getHandle()];
			node->parent = parent_node;
			parent_node->children.push_back(*node);
		} else {
			root_nodes.push_back(*node);
			node->parent = nullptr;
		}
	}

	void Scene::setParent(SceneNode *node, SceneNode *parent_node) {
		removeFromTree(node);
		node->parent = parent_node;
		if (parent_node != nullptr) {
			node->parent = parent_node;
			parent_node->children.push_back(*node);
		} else {
			root_nodes.push_back(*node);
		}
	}

	SceneNode *Scene::getParent(Entity entity) {
		SceneNode *node = node_map[entity.getHandle()];
		if (node->parent != nullptr) {
			return node->parent;
		}
		return nullptr;
	}

	const std::vector<SceneNode> &Scene::getChildren(Entity entity) {
		return node_map[entity.getHandle()]->children;
	}

	void Scene::printNode(SceneNode &node, int level) {
		std::string indent = "";
		for (int i = 0; i < level; ++i) {
			indent += '\t';
		}
		Log::message(indent + std::to_string(node.entity.getHandle()));
		for (auto &child : node.children) {
			printNode(child, level + 1);
		}
	}

	void Scene::print() {
		for (auto &node : root_nodes) {
			printNode(node, 0);
		}
	}
}