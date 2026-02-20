#pragma once

#include <map>
#include "Core.h"
#include "ComponentPool.h"
#include "bitset"
#include "unordered_map"
#include "dependencies/utils-collection/RawVector.h"

namespace HBE {
	class HB_API RegistryPage {
		RawVector<RawComponentPool *> component_pools;
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_of_entity[REGISTRY_PAGE_SIZE];
	public:
		size_t offset;
		uint32_t count = 0;

		bool valid_entities[REGISTRY_PAGE_SIZE];


		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> components_signature = 0;

		RegistryPage(size_t page_index) : offset(page_index * REGISTRY_PAGE_SIZE) {
			for (int i = 0; i < REGISTRY_PAGE_SIZE; ++i) {
				valid_entities[i] = false;
				components_of_entity[i] = 0;
			}
		};

		size_t handleToIndex(entity_handle handle) {
			return handle - offset;
		}

		RawComponentPool *getRawPool(size_t bit) {
			return bit < component_pools.size() ? component_pools[bit] : nullptr;
		}

		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> &getSignature(entity_handle handle) {
			return components_of_entity[handleToIndex(handle)];
		}

		template<typename Component>
		Component *attach(entity_handle handle, ComponentTypeInfo &type) {
			size_t i = handleToIndex(handle);
			HB_ASSERT(valid_entities[i], "Enitty#" + std::to_string(handle) + "is not valid");

			components_of_entity[i].set(type.index);

			if (component_pools.size() <= type.index) {
				component_pools.resize(type.index + 1, nullptr);
			}
			if (component_pools[type.index] == nullptr) {
				component_pools[type.index] = new RawComponentPool(type, offset);
				components_signature.set(type.index, true);
			}
			Component component{};
			char *raw_ptr = component_pools[type.index]->getMemory(handle);
			Component *ptr = new(raw_ptr) Component{};
			return ptr;
		};

		void destroyAll() {
			for (int i = 0; i<component_pools.size(); ++i) {
				if(component_pools[i]==nullptr) continue;
				for (int j = 0; j < component_pools[i]->handles.size(); ++j) {
					component_pools[i]->detach(component_pools[i]->handles[j]);
				}
				for (int j = 0; j < REGISTRY_PAGE_SIZE; ++j) {
					valid_entities[j] = false;
				}

				component_pools[i]->handles.clear();

			}
		}

		void detach(entity_handle handle, ComponentTypeInfo &type) {
			size_t i = handleToIndex(handle);

			components_of_entity[i].set(type.index, false);
			if (valid_entities[i]) {
				if (component_pools[type.index] == nullptr) {
					return;
				}
				RawComponentPool *raw_pool = component_pools[type.index];
				raw_pool->detach(handle);
				if (raw_pool->handles.size() == 0) {
					components_signature.set(raw_pool->info.index, false);

					component_pools[type.index] = nullptr;
					delete raw_pool;

				}
			}
		};

		void setValid(entity_handle handle) {
			valid_entities[handleToIndex(handle)] = true;
		}

		void setInvalid(entity_handle handle) {
			size_t i = handleToIndex(handle);
			std::list<size_t> obsolete_types_hash;
			for (int bit = 0; bit < component_pools.size(); ++bit) {
				RawComponentPool *raw_pool = component_pools[bit];
				if (raw_pool != nullptr && component_pools[bit]->has(handle)) {
					detach(handle, component_pools[bit]->info);
				}
			}
			components_of_entity[i].reset();
			valid_entities[i] = false;
		}

		bool valid(entity_handle handle) {
			return valid_entities[handle - offset];
		}


	};
}