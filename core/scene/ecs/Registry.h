#pragma once

#include "Core.h"
#include "queue"
#include "list"
#include "map"
#include "bitset"

#include "set"
#include "core/utility/Log.h"
#include <cstring>
#include "algorithm"
#include "string"
#include "core/utility/Profiler.h"
#include "Group.h"
#include "ComponentPool.h"
#include "ComponentTypeRegistry.h"

namespace HBE
{
	template <typename... Components>
	class Group;

	class HB_API Registry
	{
		uint32_t current_handle = 0;
		std::queue<entity_handle> inactive;
		std::vector<RegistryPage*> pages = std::vector<RegistryPage*>();
		std::vector<ComponentTypeInfo> types;
		std::bitset<REGISTRY_MAX_COMPONENT_TYPES> initialized_types;


		inline size_t getPage(entity_handle handle)
		{
			return (handle - (handle % REGISTRY_PAGE_SIZE)) / REGISTRY_PAGE_SIZE;
		}

	public:
		ComponentTypeRegistry type_registry;

		Registry() = default;

		~Registry();

		template <typename Component>
		void initType(size_t signature_bit)
		{
			if (!initialized_types.test(signature_bit))
			{
				const char* component_name = typeid(Component).name();
				Log::debug("init type " + (component_name + (" with signature bit " + std::to_string(signature_bit))));
				ComponentTypeInfo info = ComponentTypeInfo{signature_bit, sizeof(Component), component_name};
				if (types.size() <= signature_bit)
					types.resize(signature_bit + 1);
				types[signature_bit] = info;
				initialized_types.set(signature_bit);
			}
		}

		entity_handle create();

		void group(std::vector<entity_handle>& entities, size_t signature_bit);

		std::bitset<REGISTRY_MAX_COMPONENT_TYPES>& getSignature(entity_handle handle);

		template <typename... Components>
		typename Group<Components...> group()
		{
			constexpr size_t size = sizeof...(Components);
			size_t signature_bits[size] = {type_registry.getSignatureBit<Components>()...};
			for (int i = 0; i < size; ++i)
			{
				if (!initialized_types.test(signature_bits[i]))
				{
					return Group<Components...>(pages);
				}
			}
			ComponentTypeInfo ts[size] = {types[type_registry.getSignatureBit<Components>()]...};
			return Group<Components...>(pages, ts);
		};

		bool has(entity_handle handle, size_t signature_bit)
		{
			HB_ASSERT(valid(handle), "Entity does not exist");
			auto page = pages[getPage(handle)];
			RawComponentPool* raw_pool = page->getRawPool(signature_bit);

			if (raw_pool != nullptr)
				return raw_pool->has(handle);
			else
				return false;
		}

		template <typename Component>
		bool has(entity_handle handle)
		{
			size_t signature_bit = type_registry.getSignatureBit<Component>();
			initType<Component>(signature_bit);

			HB_ASSERT(valid(handle), "Entity does not exist");
			HB_ASSERT(initialized_types.test(signature_bit), "component " + typeName<Component>() + " is not initialized");
			return has(handle, signature_bit);
		}

		template <typename Component>
		Component* get(entity_handle handle)
		{
			size_t signature_bit = type_registry.getSignatureBit<Component>();


			initType<Component>(signature_bit);

			HB_ASSERT(initialized_types.test(signature_bit), "component" + typeName<Component>() + " is not initialized");
			HB_ASSERT(has<Component>(handle),
			          std::string("tried to get component ") + typeName<Component>() + "with signature bit " + std::to_string(type_registry.getSignatureBit<Component>()) +
			          " in entity#" +
			          std::to_string(handle) +
			          std::string(" but has<") + typeName<Component>() + ">(" + std::to_string(handle) +
			          ") == false");

			return pages[getPage(handle)]->getRawPool(signature_bit)->template getAs<Component>(handle);
		}

		template <typename Component>
		Component& get(entity_handle handle, size_t signature_bit)
		{
			HB_ASSERT(signature_bit < REGISTRY_MAX_COMPONENT_TYPES, "component bit is too large");
			initType<Component>(signature_bit);
			HB_ASSERT(initialized_types.test(signature_bit), "component " + typeName<Component>() + " is not initialized");
			HB_ASSERT(has<Component>(handle),
			          std::string("tried to get component ") + typeName<Component>() + "with signature bit " + std::to_string(type_registry.getSignatureBit<Component>()) +
			          " in entity#" +
			          std::to_string(handle) +
			          std::string(" but has<") + typeName<Component>() + ">(" + std::to_string(handle) +
			          ") == false");

			return pages[getPage(handle)]->getRawPool(signature_bit)->template getAs<Component>(handle);
		}


		void destroy(entity_handle handle);

		bool valid(entity_handle handle);

		template <typename Component>
		Component* attach(entity_handle handle)
		{
			const size_t signature_bit = type_registry.getSignatureBit<Component>();
			initType<Component>(signature_bit);
			size_t page = getPage(handle);
			return pages[page]->attach<Component>(handle, types[signature_bit]);
		}

		template <typename Component>
		void detach(entity_handle handle)
		{
			const size_t signature_bit = type_registry.getSignatureBit<Component>();
			initType<Component>(signature_bit);
			size_t page = getPage(handle);
			pages[page]->detach(handle, types[signature_bit]);
		}
	};
}
