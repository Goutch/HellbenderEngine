#pragma once

#include "Core.h"

#define OTHER_REGISTRY

#ifdef USE_ENTT

#include "entt.hpp"

#else
#ifdef OTHER_REGISTRY

#include "core/scene/ecs/PersistentRegistry/Registry.h"

#else
#include "core/scene/ecs/Registry.h"
#endif
#endif
#include "Components.h"
#include "System.h"
#include "unordered_map"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "core/scene/systems/CameraControllerSystem.h"
#include "typeinfo"


namespace HBE {
#ifdef USE_ENTT
	typedef entt::entity entity_handle;
#endif

	class Scene;

	class HB_API Entity {
#ifdef USE_ENTT
		entity_handle handle = entt::null;
#else
		entity_handle handle;
#endif
		Scene *scene = nullptr;
	public:

		Entity() = default;
		Entity(entity_handle handle, Scene *scene);

		Entity(const Entity &other);

		template<typename Component>
		Component &attach();

		template<typename Component>
		Component &attach(Component &component);
		template<typename Component>
		Component &get();

		template<typename Component>
		void detach();
		template<typename Component>
		bool has();

		bool valid();

		entity_handle getHandle();
	};

	class HB_API Scene {
	public:
		Event<> onRender;
		Event<> onDraw;
		Event<float> onUpdate;
	private:
#ifdef USE_ENTT
		public:
			entt::registry registry;
		private:
#else
		Registry registry;
#endif
		Scene(const Scene &scene) = delete;
		Scene(Scene &scene) = delete;

		std::vector<System *> systems;
		Entity main_camera_entity;

		std::unordered_map<size_t, Event<Entity>> attach_events;
		std::unordered_map<size_t, Event<Entity>> detach_events;

	public:
		Scene();
		~Scene();
		void update(float deltaTime);
		void draw();
		void render();
		Entity createEntity(const std::string &name);
		Entity createEntity();
		void destroyEntity(Entity entity);
		Entity getCameraEntity();
		void setCameraEntity(Entity camera);
		void addSystem(System *system);

		template<typename ... Components>
		auto group();
		template<typename Component>
		Component &get(entity_handle handle);

		template<typename Component>
		Component &attach(entity_handle handle);

		template<typename Component>
		Component &attach(entity_handle handle, Component &component);


		template<typename Component>
		bool has(entity_handle handle);

		template<typename Component>
		Component &detach(entity_handle handle);

		bool valid(entity_handle handle);

		template<typename Component>
		Event<Entity> &onAttach();
		template<typename Component>
		Event<Entity> &onDetach();
		template<typename Component>
		Component *getAll();
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
	void Entity::detach() {
		scene->detach<Component>(handle);
	}

	template<typename Component>
	bool Entity::has() {
		return scene->has<Component>(handle);
	}


	template<typename ... Components>
	auto Scene::group() {
#ifdef USE_ENTT
		return registry.group<Components...>();
#else
		return registry.group<Components...>();
#endif
	}


	template<typename Component>
	Component &Scene::get(entity_handle handle) {
#ifdef USE_ENTT
		return registry.get<Component>(handle);
#else
		return registry.get<Component>(handle);
#endif
	}

	template<typename Component>
	Component *Scene::getAll() {
#ifdef USE_ENTT
		return nullptr;
#else
#ifdef OTHER_REGISTRY
		return nullptr;
#else
		return registry.get<Component>();
#endif

#endif
	}

	template<typename Component>
	Component &Scene::attach(entity_handle handle) {
		size_t hash = typeid(Component).hash_code();;
#ifdef USE_ENTT
		Component &component = registry.emplace<Component>(handle, Component{});
#else
		Component &component = registry.attach<Component>(handle);
#endif
		Entity e = Entity(handle, this);
		if (attach_events.find(hash) != attach_events.end())
			attach_events[hash].invoke(e);
		return component;
	};


	template<typename Component>
	Component &Scene::attach(entity_handle handle, Component &component) {
		size_t hash = typeid(Component).hash_code();
#ifdef USE_ENTT
		Component &component2 = registry.template emplace<Component>(handle, component);
#else
		Component &component2 = registry.attach<Component>(handle, component);
#endif
		if (attach_events.find(hash) != attach_events.end())
			attach_events[hash].invoke(Entity(handle, this));
		return component;
	};

	template<typename Component>
	bool Scene::has(entity_handle handle) {
#ifdef USE_ENTT
		Component *c = registry.try_get<Component>(handle);
		return c != nullptr;
#else
		return registry.has<Component>(handle);
#endif
	}

	template<typename Component>
	Component &Scene::detach(entity_handle handle) {
		size_t hash = typeid(Component).hash_code();
		Component *component;
#ifdef USE_ENTT
		component = &registry.template erase<Component>(handle, Component{});
#else
		component = &registry.attach<Component>(handle, Component{});
#endif
		if (detach_events.find(hash) != detach_events.end())
			detach_events[hash].invoke(Entity(handle, this));
		return *component;
	};

	template<typename Component>
	Event<Entity> &Scene::onAttach() {
		size_t hash = typeid(Component).hash_code();
		if (attach_events.find(hash) == attach_events.end()) {
			attach_events.emplace(hash, Event<Entity>());
		}
		return attach_events[hash];
	};

	template<typename Component>
	Event<Entity> &Scene::onDetach() {

		size_t hash = typeid(Component).hash_code();
		if (detach_events.find(hash) == detach_events.end()) {
			detach_events.emplace(hash, Event<Entity>());
		}
		return detach_events[hash];

	}

}


