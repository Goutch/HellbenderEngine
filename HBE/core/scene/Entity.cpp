#include "Entity.h"

namespace HBE {
	Entity::Entity(entt::entity handle, entt::registry *registry) : handle(handle), registry(registry) {}

	Entity::Entity(const Entity &other) {
		registry = other.registry;
		handle = other.handle;
	}
}

