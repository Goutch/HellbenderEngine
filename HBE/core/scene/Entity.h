#pragma once

#include "Core.h"
#include <cstdint>
#include "HBETypes.h"
namespace HBE {
	class HB_API Entity {
		friend class Scene;

		template<typename ... Components>
		friend class EntityGroup;

		EntityHandle handle = entt::null;
		entt::registry *registry = nullptr;
	public:

		Entity() = default;
		Entity(EntityHandle handle, entt::registry *registry);

		Entity(Entity &other);

		template<typename Component, typename ... Args>
		Component &attach(Component &c) {
			return registry->emplace_or_replace<Component>(handle);
		}

		template<typename Component, typename ... Args>
		Component &attach(Args &&... args) {
			return registry->emplace_or_replace<Component>(handle, std::forward<Args>(args)...);
		}

		template<typename Component>
		Component &get() {
			return registry->get<Component>(handle);
		}

		template<typename Component>
		void detach() {
			registry->remove<Component>(handle);
		}

		template<typename Component>
		bool has() {
			return registry->any_of<Component>(handle);
		}

		bool valid() {
			return registry->valid(handle);
		}

		EntityHandle getHandle()
		{
			return handle;
		}

	};
}


