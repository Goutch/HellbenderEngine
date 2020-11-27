//
// Created by User on 16-Nov.-2020.
//

#include "ComponentRegistry.h"

std::unordered_map<std::string, IComponentInfo *> ComponentRegistry::types;

Component *ComponentRegistry::attach(const std::string &component_name, Entity *entity) {
    return types.at(component_name)->attachTo(entity);
}

void ComponentRegistry::terminate() {
    for (auto key_value_pair:types) {
        delete key_value_pair.second;
    }
}
