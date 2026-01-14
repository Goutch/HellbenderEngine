#pragma once

#include <array>

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
#include "Component.h"

namespace HBE {
    template<typename... Components>
    class Group;

    class HB_API Registry {
        static const uint32_t MAX_REGISTRIES = 256;
        static const uint32_t INVALID_TYPE_INDEX = std::numeric_limits<uint32_t>::max();
        static std::array<bool, MAX_REGISTRIES> free_registry_ids;
        uint32_t registry_id;
        uint32_t current_type_bit = 0;
        uint32_t current_handle = 0;
        std::queue<entity_handle> inactive;
        std::vector<RegistryPage *> pages = std::vector<RegistryPage *>();
        std::vector<ComponentTypeInfo> types;
        std::bitset<REGISTRY_MAX_COMPONENT_TYPES> initialized_types;
        std::map<uint64_t, uint32_t> component_hash_to_id;

        std::vector<uint32_t *> component_types_id_caches;

    private:
        size_t getPage(entity_handle handle);

    public:
        uint32_t generateTypeIndex(uint64_t component_hash, const char *component_name);

        uint32_t findValidRegistryIndex();

        Registry();

        ~Registry();

        entity_handle create();

        void group(std::vector<entity_handle> &entities, size_t type_index);

        std::bitset<REGISTRY_MAX_COMPONENT_TYPES> &getSignature(entity_handle handle);

        void destroy(entity_handle handle);

        bool valid(entity_handle handle);

        template<typename T>
        struct is_valid_component {
        private:
            template<typename T>
            static auto test(int) -> decltype(
                (void) T::COMPONENT_HASH, // checks existence of Key
                (void) T::COMPONENT_NAME, // checks existence of Name
                std::true_type{}
            );

            template<typename>
            static std::false_type test(...);

        public:
            static constexpr bool value = decltype(test<T>(0))::value;
        };



        template<typename Component>
        void initType(uint32_t typeIndex) {
            if (!initialized_types.test(typeIndex)) {
                ComponentTypeInfo info = ComponentTypeInfo{
                    typeIndex, sizeof(Component), Component::COMPONENT_NAME, Component::COMPONENT_HASH
                };

                types.push_back(info);
                initialized_types.set(typeIndex);
            }
        }

        template<typename ComponentType>
        uint32_t getTypeIndex() {
            static_assert(is_valid_component<ComponentType>::value, "Component structs must have COMPONENT_IDS(Type) macro inside the struct!");
            //global cache for this type T, keep the index of the type per registry id.
            static std::array<uint32_t, MAX_REGISTRIES> cache = [] {
                std::array<uint32_t, MAX_REGISTRIES> a{};
                a.fill(INVALID_TYPE_INDEX);
                return a;
            }();

            uint32_t index = cache[registry_id];
            if (index != INVALID_TYPE_INDEX)
                return index;

            //first dll/exe use this component type
            index = generateTypeIndex(ComponentType::COMPONENT_HASH, ComponentType::COMPONENT_NAME);
            component_types_id_caches.emplace_back(&cache[registry_id]);
            cache[registry_id] = index;
            initType<ComponentType>(index);
            return cache[registry_id];
        }


        template<typename... Components>
        typename Group<Components...> group() {
            constexpr size_t size = sizeof...(Components);
            size_t signature_bits[size] = {getTypeIndex<Components>()...};
            for (int i = 0; i < size; ++i) {
                if (!initialized_types.test(signature_bits[i])) {
                    return Group<Components...>(pages);
                }
            }
            ComponentTypeInfo ts[size] = {types[getTypeIndex<Components>()]...};
            return Group<Components...>(pages, ts);
        };

        bool has(entity_handle handle, size_t signature_bit) {
            HB_ASSERT(valid(handle), "Entity does not exist");
            auto page = pages[getPage(handle)];
            RawComponentPool *raw_pool = page->getRawPool(signature_bit);

            if (raw_pool != nullptr)
                return raw_pool->has(handle);
            else
                return false;
        }

        template<typename Component>
        bool has(entity_handle handle) {
            HB_ASSERT(valid(handle), "Entity does not exist");
            HB_ASSERT(initialized_types.test(getTypeIndex<Component>()),
                      std::string("component ") + Component::COMPONENT_NAME + " is not initialized");

            return has(handle, getTypeIndex<Component>());
        }

        template<typename Component>
        Component *get(entity_handle handle) {
            HB_ASSERT(initialized_types.test(getTypeIndex<Component>()),
                      std::string("component") + Component::COMPONENT_NAME + " is not initialized");
            HB_ASSERT(has<Component>(handle),
                      std::string("tried to get component ") + Component::COMPONENT_NAME + "with signature bit " + std::
                      to_string(getTypeIndex<Component>()) +
                      " in entity#" +
                      std::to_string(handle) +
                      std::string(" but has<") + Component::COMPONENT_NAME + ">(" + std::to_string(handle) +
                      ") == false");

            return pages[getPage(handle)]->getRawPool(getTypeIndex<Component>())->template getAs<Component>(handle);
        }

        template<typename Component>
        Component &get(entity_handle handle, size_t type_index) {
            HB_ASSERT(type_index < REGISTRY_MAX_COMPONENT_TYPES, "component bit is too large");
            HB_ASSERT(has<Component>(handle),
                      std::string("tried to get component ") + Component::COMPONENT_NAME + "with signature bit " + std::
                      to_string(getTypeIndex<Component>()) +
                      " in entity#" +
                      std::to_string(handle) +
                      std::string(" but has<") + Component::COMPONENT_NAME + ">(" + std::to_string(handle) +
                      ") == false");

            return pages[getPage(handle)]->getRawPool(type_index)->template getAs<Component>(handle);
        }

        template<typename Component>
        Component *attach(entity_handle handle) {
            uint32_t type_index = getTypeIndex<Component>();
            size_t page = getPage(handle);
            return pages[page]->attach<Component>(handle, types[type_index]);
        }

        template<typename Component>
        void detach(entity_handle handle) {
            uint32_t type_index = getTypeIndex<Component>();

            size_t page = getPage(handle);
            pages[page]->detach(handle, types[type_index]);
        }
    };
}
