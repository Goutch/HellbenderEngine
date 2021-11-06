#pragma once

#include "Core.h"
#include "entt.hpp"
#include "HBETypes.h"

namespace HBE {

	template<typename ... Components>
	class HB_API EntityGroup {
        friend class Scene;
        entt::view<entt::exclude_t<>,Components...> view;

		EntityGroup(entt::registry &registry):view(registry.view<Components...>()){

		}

	public:
        EntityGroup(EntityGroup<Components...>& other)=default;
		template<typename T>
		T &get(EntityHandle entity_handle) {
			return view.template get<T>(entity_handle);
		}

        auto begin() {
			return view.begin();
		}

        auto end() {
			return view.end();
		}

	};
}
