//
// Created by User on 16-Nov.-2020.
//

#include "ComponentRegistry.h"
namespace HBE {
    std::unordered_map<std::string, IComponentInfo *> ComponentRegistry::types;

    Component *ComponentRegistry::attach(const std::string &component_name, Entity *entity) {
        return types.at(component_name)->attachTo(entity);
    }

    void ComponentRegistry::terminate() {
        for (auto key_value_pair:types) {
            delete key_value_pair.second;
        }
    }

    std::list<std::string> ComponentRegistry::getComponentsName() {
        std::list<std::string> name_list;
        for (auto componentInfo:types) {
            name_list.push_back(componentInfo.first);
        }
        return name_list;
    }
}