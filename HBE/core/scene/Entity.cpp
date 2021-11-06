#include "Entity.h"

namespace HBE {
	Entity::Entity(entt::entity handle, entt::registry *registry) : handle(handle), registry(registry) {}

	Entity::Entity(Entity &other)  = default;
}

