#pragma once

#include "Core.h"
#include "entt.hpp"
#include "Components.h"
#include "Entity.h"
#include "EntityGroup.h"
namespace HBE {
	class Entity;
	class HB_API Scene {
		entt::registry registry;
		Entity main_camera_entity;
	public:
		Entity createEntity();
		void destroyEntity(Entity entity);

		Entity getCameraEntity();
		void setCameraEntity(Entity camera);

		template<typename ... Components>
		EntityGroup<Components...> group() {
			return EntityGroup<Components...>(registry);
		}
	};
}


