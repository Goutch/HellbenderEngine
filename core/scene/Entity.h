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
		Component *attach(Component &component);

		template<typename Component>
		Component *get();

		template<typename Component>
		Component *get(size_t signature_bit);

		template<typename Component>
		void detach();

		template<typename Component>
		bool has();

		bool has(size_t signature_bit);

		bool valid();

		entity_handle getHandle() const;

		int operator==(const Entity &other) const;
		int operator!=(const Entity &other) const;

	};
}
