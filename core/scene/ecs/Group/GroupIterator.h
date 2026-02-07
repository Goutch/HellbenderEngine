#pragma once

#include "core/scene/ecs/Registry/Registry.h"
#include "core/scene/ecs/RawVector.h"
#include "core/scene/ecs/Group/ArchetypeData.h"

namespace HBE {
	class Registry;

	template<typename... Components>
	class GroupIterator {
		RawVector<ArchetypeData<Components...>> &pools_data;
		RawVector<EntityRef> *page_entity_references;//precomputed entities with the group signature
		uint32_t page_count;
		size_t current_entity_ref_index = 0;
		size_t current_page = 0;

	public:

		GroupIterator(RawVector<ArchetypeData<Components...>> &archetype_data,
		              RawVector<EntityRef> *page_entity_references,
		              size_t page_count,
		              size_t start_page = 0,
		              size_t start_index = 0) :
				pools_data(archetype_data),
				page_entity_references(page_entity_references),
				page_count(page_count) {
			this->current_entity_ref_index = start_index;
			this->current_page = start_page;

		}

		std::tuple<entity_handle, Components &...> operator*() {
			return pools_data[current_page].createTuple(page_entity_references[current_page][current_entity_ref_index].handle,
			                                            page_entity_references[current_page][current_entity_ref_index].index,
			                                            std::index_sequence_for<Components...>{});
		}

		GroupIterator &operator++() {
			current_entity_ref_index++;
			while (current_page < page_count &&
			       current_entity_ref_index >= page_entity_references[current_page].size()) {
				current_entity_ref_index = 0;
				do {
					current_page++;
				} while (current_page < page_count && page_entity_references[current_page].size() == 0);
			}
			return *this;
		}


		bool operator==(const GroupIterator<Components...> &other) const {
			return current_entity_ref_index == other.current_entity_ref_index &&
			       current_page == other.current_page;
		}

		bool operator!=(const GroupIterator<Components...> &other) const {
			return current_page != other.current_page ||
			       current_entity_ref_index != other.current_entity_ref_index;
		}

	};
};
