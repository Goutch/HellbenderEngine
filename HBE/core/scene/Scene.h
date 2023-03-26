#pragma once

#include "Core.h"

#include "core/scene/ecs/Registry.h"
#include "unordered_map"

#include "core/utility/Event.h"
#include "core/graphics/RenderGraph.h"

#include "core/graphics/Graphics.h"
#include "core/resource/RenderTarget.h"
#include "Application.h"

#include "core/scene/Entity.h"
#include "core/scene/components/Transform.h"
#include "core/scene/components/Transform2D.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/ModelRenderer.h"
#include "core/scene/components/MeshRenderer.h"


namespace HBE {
	struct SceneNode {
		Entity entity;
		bool has_parent = false;
		SceneNode *parent = nullptr;
		std::list<SceneNode> children;

		bool operator==(const SceneNode &other) const {
			return entity.getHandle() == other.entity.getHandle();
		}

	};
	class CameraSystem;

	class ModelRendererSystem;

	class MeshRendererSystem;

	class CameraControllerSystem;

	class System;

	class HB_API Scene {

	public:
		Event<RenderGraph *> onRender;
		Event<RenderGraph *> onDraw;
		Event<float> onUpdate;
		Event<Scene *> onSceneActivate;
		Event<Scene *> onSceneDeactivate;

	private:
		std::vector<System *> systems;
		bool is_active = true;
		Registry registry;
		RenderGraph render_graph;
		std::list<SceneNode> root_nodes;

		std::unordered_map<entity_handle, SceneNode *> node_map;

		Scene(const Scene &scene) = delete;

		Scene(Scene &scene) = delete;

		Entity main_camera_entity;
		std::unordered_map<size_t, Event<Entity>> attach_events;
		std::unordered_map<size_t, Event<Entity>> detach_events;


	public:

		void setActive(bool active);

		bool isActive();

		virtual Texture *getMainCameraTexture();

		Scene();

		virtual ~Scene();

		void update(float delta_t);

		void draw();

		void render();

		Entity createEntity3D();

		Entity createEntity2D();

		Entity createEntity();

		void destroyEntity(entity_handle entity);

		Entity getCameraEntity();

		void setCameraEntity(Entity camera);

		void addSystem(System *system);

		template<typename ... Components>
		Group<Components...> group();

		std::list<SceneNode> getSceneNodes();

		template<typename Component>
		Component &get(entity_handle handle);

		template<typename Component>
		Component &get(entity_handle handle, size_t signature_bit);

		template<typename Component>
		size_t getComponentSignatureBit();

		template<typename Component>
		Component &attach(entity_handle handle);

		template<typename Component>
		Component &attach(entity_handle handle, Component &component);

		template<typename Component>
		bool has(entity_handle handle);

		bool has(entity_handle handle, size_t signature_bit);

		template<typename Component>
		void detach(entity_handle handle);

		bool valid(entity_handle handle);

		template<typename Component>
		Event<Entity> &onAttach();

		template<typename Component>
		Event<Entity> &onDetach();

		//sceneHierachy

		void setParent(Entity entity, Entity parent = {});

		Entity getParent(Entity entity);

		const std::list<SceneNode> &getChildren(Entity entity);

		void printSceneHierarchy();

		SceneNode *getNode(Entity entity);

	private:


		void printNode(SceneNode &node, int level);

		void onAttachTransform(Entity entity);

		void setChildrenDirty(SceneNode *node);

		void onFrameChange(uint32_t frame);

		void addDefaultSystems();
	};

	template<typename Component>
	Component &Entity::attach(Component &component) {
		return scene->attach<Component>(handle, component);
	}

	template<typename Component>
	Component &Entity::attach() {
		return scene->attach<Component>(handle);
	}

	template<typename Component>
	Component &Entity::get() {
		return scene->get<Component>(handle);
	}

	template<typename Component>
	Component &Entity::get(size_t signature_bit) {
		return scene->get<Component>(handle, signature_bit);
	}

	template<typename Component>
	void Entity::detach() {
		scene->detach<Component>(handle);
	}

	template<typename Component>
	bool Entity::has() {
		return scene->has<Component>(handle);
	}

	template<typename ... Components>
	Group<Components...> Scene::group() {
		return registry.group<Components...>();
	}


	template<typename Component>
	Component &Scene::get(entity_handle handle) {
		return registry.get<Component>(handle);
	}

	template<typename Component>
	Component &Scene::get(entity_handle handle, size_t id) {
		return registry.get<Component>(handle, id);
	}

	template<typename Component>
	Component &Scene::attach(entity_handle handle) {
		Component &component = registry.attach<Component>(handle);
		Entity e = Entity(handle, this);
		size_t bit = registry.type_registry.getSignatureBit<Component>();
		if (attach_events.find(bit) != attach_events.end())
			attach_events[bit].invoke(e);
		return component;
	};


	template<typename Component>
	Component &Scene::attach(entity_handle handle, Component &component) {
		size_t hash = registry.type_registry.getSignatureBit<Component>();
		Component &component_ref = registry.attach<Component>(handle, component);

		if (attach_events.find(hash) != attach_events.end())
			attach_events[hash].invoke(Entity(handle, this));
		return component_ref;
	};

	template<typename Component>
	bool Scene::has(entity_handle handle) {
		return registry.has<Component>(handle);
	}


	template<typename Component>
	void Scene::detach(entity_handle handle) {
		size_t bit = registry.type_registry.getSignatureBit<Component>();

		if (detach_events.find(bit) != detach_events.end())
			detach_events[bit].invoke(Entity(handle, this));
		registry.detach<Component>(handle);
	};

	template<typename Component>
	Event<Entity> &Scene::onAttach() {
		size_t bit = registry.type_registry.getSignatureBit<Component>();
		registry.initType<Component>(bit);
		if (attach_events.find(bit) == attach_events.end()) {
			attach_events.emplace(bit, Event<Entity>());
		}
		return attach_events[bit];
	};

	template<typename Component>
	Event<Entity> &Scene::onDetach() {

		size_t bit = registry.type_registry.getSignatureBit<Component>();
		if (detach_events.find(bit) == detach_events.end()) {
			detach_events.emplace(bit, Event<Entity>());
		}
		return detach_events[bit];

	}

	template<typename Component>
	size_t Scene::getComponentSignatureBit() {
		return registry.type_registry.getSignatureBit<Component>();
	}


}


