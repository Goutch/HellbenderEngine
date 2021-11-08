#pragma once

#include "Core.h"
#include "entt.hpp"
#include "Components.h"
#include "Entity.h"

#include "System.h"
#include "unordered_map"
#include "core/scene/systems/CameraSystem.h"
#include "core/scene/systems/MeshRendererSystem.h"
#include "typeinfo"

namespace HBE {
	class Entity;

	template<typename ... Components>
	using EntityGroup = entt::basic_view<EntityHandle, entt::exclude_t<>, Components...>;

	class HB_API Scene {
	public:
		Event<> onRender;
		Event<> onDraw;
		Event<float> onUpdate;
	private:
		friend class entt::basic_registry<EntityHandle>;

		entt::registry registry;
		std::unordered_map<std::string, Entity> entities;

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
		Entity get(const std::string &entity_name);
		Entity createEntity(const std::string &name);
		Entity createEntity();
		void destroyEntity(Entity entity);
		Entity getCameraEntity();
		void setCameraEntity(Entity camera);

		template<typename ... Components>
		EntityGroup<Components...> group() {
			auto view = registry.view<Components...>();
			return view;
		}

		template<typename Component>
		Entity get(Component &component) {
			return Entity(entt::to_entity(registry, component), registry);
		}

		template<typename Component>
		Event<Entity> &onAttach() {
			size_t hash = typeid(Component).hash_code();
			if (attach_events.find(hash)==attach_events.end()) {
				attach_events.emplace(hash, Event<Entity>());
				registry.on_construct<Component>().template connect<&Scene::onComponentAttached<Component>>(this);
			}
			return attach_events[hash];
		};

		template<typename Component>
		Event<Entity> &onDetach() {
			size_t hash = typeid(Component).hash_code();
			if (detach_events.find(hash)==detach_events.end()) {
				detach_events.emplace(hash, Event<Entity>());
				registry.on_destroy<Component>().template connect<&Scene::onComponentAttached<Component>>(this);
			}
			return detach_events[hash];
		};

	private:
		template<typename Component>
		void onComponentAttached(entt::registry &registry, EntityHandle entity_handle) {
			size_t hash = typeid(Component).hash_code();
			attach_events[hash].invoke(Entity(entity_handle, &registry));
		}

		template<typename Component>
		void onComponentDetached(entt::registry &registry, EntityHandle entity_handle) {
			size_t hash = typeid(Component).hash_code();
			detach_events[hash].invoke(Entity(entity_handle, &registry));
		}
	};


}


