#pragma once

#include "Core.h"
#include "ecs/Registry.h"

namespace HBE {
	class Scene;

	class HB_API Entity {
		entity_handle handle = std::numeric_limits<entity_handle>::max();
		Scene *scene = nullptr;

	public:

		Entity() = default;

		Entity(entity_handle handle, Scene *scene);

		Entity(const Entity &other);

		Scene* getScene();
		template<typename Component>
		Component &attach();

		template<typename Component>
		Component &attach(Component &component);

		template<typename Component>
		Component &get();

		template<typename Component>
		Component &get(component_type_id id);

		template<typename Component>
		void detach();

		template<typename Component>
		bool has();

		bool has(component_type_id id);

		bool valid();

		entity_handle getHandle() const;

		Entity getParent() const;
	};
}
