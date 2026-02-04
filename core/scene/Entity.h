#pragma once

#include "Core.h"
#include "ecs/ComponentPool.h"

namespace HBE {
	class Scene;

	class HB_API Entity {
	public:
		static const Entity NULL_ENTITY;
	private:
		entity_handle handle = std::numeric_limits<entity_handle>::max();
		Scene *scene = nullptr;

	public:
		Entity() = default;

		Entity(entity_handle handle, Scene *scene);

		Entity(const Entity &other);

		void destroy();

		Scene *getScene();

		template<typename Component>
		Component *attach();

		template<typename Component>
		Component *attach(const Component &component);

		template<typename Component>
		Component *get();

		template<typename Component>
		Component *get(uint32_t type_index);

		template<typename Component>
		void detach();

		template<typename Component>
		bool has();

		bool has(uint32_t type_index);

		bool valid();

		entity_handle getHandle() const;

		int operator==(const Entity &other) const;
		int operator!=(const Entity &other) const;

	};
}
