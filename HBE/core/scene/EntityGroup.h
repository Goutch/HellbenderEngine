#pragma once

#include "Core.h"
#include "entt.hpp"
#include "HBETypes.h"

namespace HBE {

	template<typename ... Components>
	class HB_API EntityGroup {
		friend class Scene;

		entt::basic_view<Components...> view;

		EntityGroup(entt::registry &registry) : view(registry.view<Components...>()) {

		}

	public:
		template<typename Component>
		Component &get(EntityHandle entity_handle) {
			return view.get(entity_handle);
		}

		EntityHandle *begin() {
			return view.begin();
		}

		EntityHandle *end() {
			return view.end();
		}

	};
}
