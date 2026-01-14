//
// Created by User on 3/24/2023.
//

#pragma once


#include <cstdint>
#include "string"
#include "Core.h"
#include "vector"
#include "typeinfo"
#include "core/utility/Log.h"
#include "algorithm"

#ifndef REGISTRY_PAGE_SIZE
#define REGISTRY_PAGE_SIZE 1024
#endif
#ifndef REGISTRY_MAX_COMPONENT_TYPES
#define REGISTRY_MAX_COMPONENT_TYPES 128
#endif
#define NULL_ENTITY_HANDLE UINT64_MAX

namespace HBE {
    typedef uint64_t entity_handle;

    struct ComponentTypeInfo {

        uint32_t index;
        size_t size;
        std::string name;
        uint64_t hash;

        bool operator==(const ComponentTypeInfo &other) const {
            return index == other.index && hash == other.hash;
        }
    };

    class HB_API RawComponentPool {
    public:
        ComponentTypeInfo info;
        char *data;
        bool valid[REGISTRY_PAGE_SIZE];
        std::vector<entity_handle> handles;
        size_t offset;

        RawComponentPool(RawComponentPool &&other);

        RawComponentPool(ComponentTypeInfo info, size_t offset);

        ~RawComponentPool();

        char *getMemory(entity_handle handle);

        template<typename Component>
        Component *getAs(entity_handle handle) {
            size_t i = handle - offset;
            return reinterpret_cast<Component *>(&data[i * info.size]);
        }

        void detach(entity_handle handle);

        bool has(entity_handle handle);
    };

    template<typename Component>
    struct ComponentPool {
        Component *components = nullptr;
        size_t offset = 0;
        entity_handle *handles = nullptr;
        std::size_t count = 0;

        ComponentPool() = default;

        explicit ComponentPool(RawComponentPool &page) {
            components = reinterpret_cast<Component *>(&page.data[0]);
            count = page.handles.size();
            offset = page.offset;
        }

        Component &operator[](size_t handle) {
            return components[handle - offset];
        }
    };
}
