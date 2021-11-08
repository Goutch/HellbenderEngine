#pragma once

#include "Core.h"
#include "entt.hpp"
#include "HBETypes.h"

namespace HBE {

	/*template<typename ... Components>
	class EntityGroup: public entt::view<Components...>{
        friend class Scene;
		//entt::basic_view<EntityHandle , entt::exclude_t<>, Components...> entity_view;

		public:
        EntityGroup(EntityGroup<Components...>& other):entity_view(other.view){};
		template<typename T>
		T &get(EntityHandle entity_handle) {
			return entity_view.template get<T>(entity_handle);
		}

        auto begin() {
			return entity_view.begin();
		}

        auto end() {
			return entity_view.end();
		}

	};*/
}
