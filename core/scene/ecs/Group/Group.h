#pragma once

#include "Core.h"
#include "core/scene/ecs/Registry/Registry.h"
#include "GroupIterator.h"
#include "core/scene/ecs/RawVector.h"
#include "core/scene/ecs/Group/ArchetypeData.h"

namespace HBE {

	template<typename... Components>
	class GroupIterator;

	template<typename... Components>
	class Group {
		Registry *registry;
		RawVector<ArchetypeData<Components...>> pools_data;
		RawVector<EntityRef> *page_entity_references;
		size_t page_count = 0;
	public:


		Group(Registry *registry) : registry(registry) {

			bool empty = true;
			ComponentTypeInfo types[sizeof ...(Components)];
			registry->getComponentsTypeInfo<Components...>(types);

			RawVector<RegistryPage *> &pages = registry->getPages();
			pools_data.reserve(pages.size());

			std::bitset<REGISTRY_MAX_COMPONENT_TYPES> required_signature = 0;
			for (uint32_t i = 0; i < sizeof...(Components); ++i) {
				required_signature.set(types[i].index, true);
			}
			for (uint32_t i = 0; i < pages.size(); ++i) {
				ArchetypeData<Components...> archetype_data{};

				for (int j = 0; j < sizeof...(Components); ++j) {
					RawComponentPool *pool = pages[i]->getRawPool(types[j].index);
					if (pool != nullptr) {
						archetype_data.set(j, reinterpret_cast<void *>(pool->data));
					} else {
						archetype_data.set(j, nullptr);
					}
				}
				pools_data.add(archetype_data);
			}

			page_entity_references = new RawVector<EntityRef>[pages.size()];
			page_count = pages.size();

			for (uint32_t i = 0; i < pages.size(); ++i) {
				RegistryPage *page = pages[i];

				//skip if there is no matching components in this page
				if ((page->components_signature & required_signature) != required_signature)
					continue;


				//find the pool with the least entities
				RawComponentPool *min_size_pool = nullptr;
				uint32_t min_size = UINT32_MAX;
				for (uint32_t j = 0; j < sizeof ...(Components); ++j) {
					RawComponentPool* pool = pages[i]->getRawPool(types[j].index);
					if (pool->handles.size() < min_size) {
						min_size_pool = pool;
						min_size = min_size_pool->handles.size();
					}
				}
				//create the list of entity indices for this page
				page_entity_references[i].reserve(min_size_pool->handles.size());
				RawVector<entity_handle> &handles = min_size_pool->handles;

				for (int j = 0; j < handles.size(); ++j) {

					entity_handle handle = handles[j];
					std::bitset<REGISTRY_MAX_COMPONENT_TYPES> entity_signature = page->getSignature(handle);
					if ((entity_signature & required_signature) == required_signature) {
						page_entity_references[i].add({page->handleToIndex(handle), handle});
					}
				}
				if (!page_entity_references[i].empty()) {
					empty = false;
				}
			}

			if (empty) {
				page_count = 0;
			}
		}

		~Group() {
			delete[] page_entity_references;
		};

		GroupIterator<Components...> begin() {
			return GroupIterator<Components...>(pools_data, page_entity_references, page_count);
		};

		GroupIterator<Components...> end() {
			return GroupIterator<Components...>(pools_data, page_entity_references, page_count, page_count, static_cast<size_t>(0));
		}
	};
}
