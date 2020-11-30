#pragma once

#include <string>
#include <core/entity/Entity.h>
#include "Core.h"
#include "map"
#include "unordered_map"
namespace HBE {
    struct IComponentInfo {
        virtual Component *attachTo(Entity *e) = 0;

        virtual ~IComponentInfo() {};
    };

    template<typename T>
    struct ComponentInfo : IComponentInfo {
        Component *attachTo(Entity *e) {
            return e->attach<T>();
        }
    };

    class HB_API ComponentRegistry {
        static std::unordered_map<std::string, IComponentInfo *> types;
    public:
        template<typename ComponentType>
        static void registerComponent(std::string name) {
            if (types.find(name) != types.end()) {
                types.emplace(name, new ComponentInfo<ComponentType>());
            }
        }

        static Component *attach(const std::string &component_name, Entity *entity);

        static void terminate();
    };
}
