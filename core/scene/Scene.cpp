#pragma once

#include "Scene.h"
#include "Entity.h"
#include "vector"
#include "components/EntityState.h"
#include "components/Node.h"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/CameraControllerSystem.h"
#include "core/scene/systems/ModelRendererSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "systems/NodeSystem.h"
#include "systems/TransformSystem.h"

namespace HBE {
	const Entity Entity::NULL_ENTITY = Entity();

	Entity Scene::getCameraEntity() {
		return main_camera_entity;
	}

	void Scene::destroyEntity(entity_handle entity) {
		if (!valid(entity)) return;

		Node *node = get<Node>(entity);
		for (int32_t i = node->children.size() - 1; i >= 0; --i) {
			Node *child_node = get<Node>(node->children[i]);
			child_node->local_index = node->children.size() - 1;
			destroyEntity(node->children[i]);
		}
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> signature = registry.getSignature(entity);

		for (int i = 0; i < detach_events.size(); ++i) {
			if (signature.test(i)) {
				detach_events[i].invoke(Entity(entity, this));
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
			node_system->updateNodeIndices();
		}
	}

	void Scene::draw() {
		if (is_active) {
			onDraw.invoke(&render_graph);
		}
	}

	void Scene::present() {
	}

	void Scene::render() {
		if (is_active) {
			onRender.invoke(&render_graph);
		}
	}

	Scene::~Scene() {
		for (int i = 0; i < detach_events.size(); i++) {
			if (detach_events[i].listenerCount() == 0) continue;
			RawVector<entity_handle> handle_buffer;
			registry.getAllEntitiesWith(i, handle_buffer);
			for (entity_handle handle: handle_buffer) {
				detach_events[i].invoke(Entity(handle, this));
			}
		}
		for (int i = 0; i < systems.size(); ++i) {
			delete systems[i];
		}
		Application::onDraw.unsubscribe(draw_subscription_id);
		Application::onRender.unsubscribe(render_subscription_id);
		Application::onUpdate.unsubscribe(update_subscription_id);
		Application::onPresent.unsubscribe(present_subscription_id);
	}

	bool Scene::valid(entity_handle handle) {
		return registry.valid(handle);
	}


	bool Scene::has(entity_handle handle, size_t signature_bit) {
		return registry.has(handle, signature_bit);
	}

	const RawVector<entity_handle> &Scene::getRootNodes() const {
		return node_system->getRootNodes();
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

	bool Entity::has(uint32_t type_index) {
		return scene->has(handle, type_index);
	}

	Scene *Entity::getScene() {
		return scene;
	}

	void Entity::destroy() {
		scene->destroyEntity(handle);
	}

	int Entity::operator==(const Entity &other) const {
		return other.handle == handle && other.scene == scene;
	}

	int Entity::operator!=(const Entity &other) const {
		return !(*this == other);
	}

	void Scene::addSystem(System *system) {
		systems.push_back(system);
	}

	Scene::Scene(SceneInfo info) {
		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_TRANSFORM_SYSTEM) {
			transform_system = new TransformSystem(this);
			addSystem(transform_system);
		} else {
			Log::warning("Transform System disabled, it is mandatory for Transform.world() function to work properly");
		}

		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_NODE_SYSTEM) {
			node_system = new NodeSystem(this);
			addSystem(node_system);
		} else {
			Log::warning("Node System disabled, it is mandatory for Transform.world() function to work properly");
		}

		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_CAMERA_SYSTEM)
			addSystem(new CameraSystem(this));
		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_CAMERA_CONTROLLER_SYSTEM)
			addSystem(new CameraControllerSystem(this));
		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_MODEL_RENDERER_SYSTEM)
			addSystem(new ModelRendererSystem(this));
		if (info.initialized_systems_flags & SCENE_INITIALIZE_SYSTEMS_FLAG_MESH_RENDERER_SYSTEM)
			addSystem(new MeshRendererSystem(this));

		Application::onDraw.subscribe(draw_subscription_id, this, &Scene::draw);
		Application::onRender.subscribe(render_subscription_id, this, &Scene::render);
		Application::onUpdate.subscribe(update_subscription_id, this, &Scene::update);
		Application::onPresent.subscribe(present_subscription_id, this, &Scene::present);
		Graphics::onFrameChange.subscribe(frame_change_subscription_id, this, &Scene::onFrameChange);
	}

	Entity Scene::createEntity() {
		Entity e = Entity(registry.create(), this);
		e.attach<EntityState>();
		return e;
	}

	Entity Scene::createEntity3D() {
		Entity e = createEntity();
		HB_ASSERT(node_system != nullptr,
		          "Node system is not initialized, node3D components will not work properly");
		Node node = Node(e);
		e.attach<Node>(node);;

		HB_ASSERT(transform_system != nullptr,
		          "Transform system is not initialized, transform.world() function will not work properly");
		e.attach<Transform>();


		return e;
	}

	Entity Scene::createEntity2D() {
		Entity e = createEntity();
		HB_ASSERT(node_system != nullptr,
		          "Node system is not initialized, node2D components will not work properly");
		e.attach<Node>();
		HB_ASSERT(transform_system != nullptr,
		          "Transform system is not initialized, transform.world() function will not work properly");
		e.attach<Transform2D>();
		return e;
	}

	void Scene::setParent(Entity entity, Entity parent) {
		setParent(entity.getHandle(), parent.getHandle());
	}

	void Scene::setParent(entity_handle entity, entity_handle parent) {
		node_system->setParent(entity, parent);
	}

	Image *Scene::getMainCameraTexture() {
		if (main_camera_entity.valid() && (main_camera_entity.has<Camera>() || main_camera_entity.has<Camera2D>() ||
		                                   main_camera_entity.has<PixelCamera>())) {
			if (main_camera_entity.valid() && (main_camera_entity.has<Camera>())) {
				return main_camera_entity.get<Camera>()->getRenderTarget()->getFramebufferTexture(
						Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<Camera2D>())) {
				return main_camera_entity.get<Camera2D>()->getRenderTarget()->getFramebufferTexture(
						Graphics::getCurrentFrame());
			}
			if (main_camera_entity.valid() && (main_camera_entity.has<PixelCamera>())) {
				return main_camera_entity.get<PixelCamera>()->getRenderTarget()->getFramebufferTexture(
						Graphics::getCurrentFrame());
			}
		}
		return nullptr;
	}
}
