#pragma once

#include "Core.h"

#include "core/scene/ecs/Registry.h"
#include "unordered_map"

#include "core/graphics/RenderGraph.h"

#include "core/graphics/Graphics.h"
#include "core/resource/RasterizationTarget.h"
#include "Application.h"

#include "core/scene/Entity.h"
#include "core/scene/components/Transform.h"
#include "core/scene/systems/TransformSystem.h"
#include "core/scene/components/Transform2D.h"
#include "core/scene/components/Camera.h"
#include "core/scene/components/Camera2D.h"
#include "core/scene/components/PixelCamera.h"
#include "core/scene/components/ModelRenderer.h"
#include "systems/NodeSystem.h"


namespace HBE {
	class CameraSystem;

	class ModelRendererSystem;

	class MeshRendererSystem;

	class CameraControllerSystem;

	class System;

	typedef uint32_t SCENE_SYSTEMS_FLAGS;

	enum SCENE_SYSTEMS_FLAG :uint32_t {
		SCENE_INITIALIZE_SYSTEMS_FLAG_NONE = 0,
		SCENE_INITIALIZE_SYSTEMS_FLAG_NODE_SYSTEM = 1 << 0,
		SCENE_INITIALIZE_SYSTEMS_FLAG_TRANSFORM_SYSTEM = 1 << 1,
		SCENE_INITIALIZE_SYSTEMS_FLAG_CAMERA_SYSTEM = 1 << 1,
		SCENE_INITIALIZE_SYSTEMS_FLAG_CAMERA_CONTROLLER_SYSTEM = 1 << 2,
		SCENE_INITIALIZE_SYSTEMS_FLAG_MODEL_RENDERER_SYSTEM = 1 << 3,
		SCENE_INITIALIZE_SYSTEMS_FLAG_MESH_RENDERER_SYSTEM = 1 << 4,
		SCENE_INITIALIZE_SYSTEMS_FLAG_ALL = UINT32_MAX,
	};

	struct SceneInfo {
		SCENE_SYSTEMS_FLAGS initialized_systems_flags = SCENE_INITIALIZE_SYSTEMS_FLAG_ALL;
	};

	class HB_API Scene {
	public:
		Event<RenderGraph *> onRender; //render camera render targets
		Event<RenderGraph *> onDraw; //called after update to record draw commands
		Event<float> onUpdate;
		Event<Scene *> onSceneActivate;
		Event<Scene *> onSceneDeactivate;

	private:
		TransformSystem *transform_system = nullptr;
		NodeSystem *node_system = nullptr;
		std::vector<System *> systems;
		bool is_active = true;
		Registry registry;
		RenderGraph render_graph;

		Scene(const Scene &scene) = delete;

		Scene(Scene &scene) = delete;

		Entity main_camera_entity;
		std::unordered_map<size_t, Event<Entity>> attach_events;
		std::unordered_map<size_t, Event<Entity>> detach_events;

	public:
		void setActive(bool active);

		bool isActive();

		virtual Image *getMainCameraTexture();

		Scene(SceneInfo info = {});

		virtual ~Scene();

		virtual void update(float delta_t);

		virtual void draw();

		virtual void present();

		virtual void render();

		Entity createEntity3D();

		Entity createEntity2D();

		Entity createEntity();

		void destroyEntity(entity_handle entity);

		Entity getCameraEntity();

		void setCameraEntity(Entity camera);

		void updateNodes(entity_handle node, uint32_t global_index);

		void addSystem(System *system);

		template<typename... Components>
		Group<Components...> group();

		const std::vector<entity_handle> &getRootNodes() const;

		template<typename Component>
		Component *get(entity_handle handle);

		template<typename Component>
		Component *get(entity_handle handle, size_t signature_bit);

		template<typename Component>
		size_t getComponentSignatureBit();

		template<typename Component>
		Component *attach(entity_handle handle);

		template<typename Component>
		Component *attach(entity_handle handle, Component &component);

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

		//sceneHierarchy

		void setParent(Entity entity, Entity parent = {});

		void setParent(entity_handle entity, entity_handle parent);

		const std::list<Node> &getChildren(Entity entity);

		void printSceneHierarchy();

	private:
		void onAttachTransform(Entity entity);

		void setChildrenDirty(Node *node);

		void onFrameChange(uint32_t frame);


		void drawNode(RenderGraph *render_graph, Node &node, int &count);
	};

	template<typename Component>
	Component *Entity::attach(Component &component) {
		return scene->attach<Component>(handle, component);
	}

	template<typename Component>
	Component *Entity::attach() {
		return scene->attach<Component>(handle);
	}

	template<typename Component>
	Component *Entity::get() {
		HB_ASSERT(scene->valid(handle), "Entity does not exist");
		HB_ASSERT(scene->has<Component>(handle), std::string("Entity#") + std::to_string(handle) + " does not have component " + Component::COMPONENT_NAME);
		return scene->get<Component>(handle);
	}

	template<typename Component>
	Component *Entity::get(uint32_t type_index) {
		return scene->get<Component>(handle, type_index);
	}

	template<typename Component>
	void Entity::detach() {
		scene->detach<Component>(handle);
	}

	template<typename Component>
	bool Entity::has() {
		return scene->has<Component>(handle);
	}

	template<typename... Components>
	Group<Components...> Scene::group() {
		return registry.group<Components...>();
	}


	template<typename Component>
	Component *Scene::get(entity_handle handle) {
		return registry.get<Component>(handle);
	}

	template<typename Component>
	Component *Scene::get(entity_handle handle, size_t id) {
		return registry.get<Component>(handle, id);
	}

	template<typename Component>
	Component *Scene::attach(entity_handle handle) {
		Component *component = registry.attach<Component>(handle);
		Entity e = Entity(handle, this);
		uint32_t index = registry.getTypeIndex<Component>();
		if (attach_events.find(index) != attach_events.end())
			attach_events[index].invoke(e);
		return component;
	};


	template<typename Component>
	Component *Scene::attach(entity_handle handle, Component &component) {
		uint32_t index= registry.getTypeIndex<Component>();
		Component &component_ref = registry.attach<Component>(handle, component);

		if (attach_events.find(index) != attach_events.end())
			attach_events[index].invoke(Entity(handle, this));
		return component_ref;
	};

	template<typename Component>
	bool Scene::has(entity_handle handle) {
		return registry.has<Component>(handle);
	}


	template<typename Component>
	void Scene::detach(entity_handle handle) {
		uint32_t index= registry.getTypeIndex<Component>();

		if (detach_events.find(index) != detach_events.end())
			detach_events[index].invoke(Entity(handle, this));
		registry.detach<Component>(handle);
	};

	template<typename Component>
	Event<Entity> &Scene::onAttach() {
		uint32_t index = registry.getTypeIndex<Component>();
		if (attach_events.find(index) == attach_events.end()) {
			attach_events.emplace(index, Event<Entity>());
		}
		return attach_events[index];
	};

	template<typename Component>
	Event<Entity> &Scene::onDetach() {
		uint32_t index = registry.getTypeIndex<Component>();
		if (detach_events.find(index) == detach_events.end()) {
			detach_events.emplace(index, Event<Entity>());
		}
		return detach_events[index];
	}

	template<typename Component>
	size_t Scene::getComponentSignatureBit() {
		return registry.getTypeIndex<Component>();
	}
}
