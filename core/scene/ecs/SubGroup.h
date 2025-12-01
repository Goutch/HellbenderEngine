#pragma once
#include "Registry.h"

namespace HBE {
    template<typename... Components>
    class SubGroup {
        RegistryPage *page;
        ComponentTypeInfo types[sizeof...(Components)];

    public:
        SubGroup(RegistryPage *page, ComponentTypeInfo types[sizeof...(Components)]) : page(page), types(types) {
        }
    };
}
