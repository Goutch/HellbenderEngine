//
// Created by User on 1/26/2024.
//

#pragma once
#include "Core.h"
namespace HBE {
    enum ENTITY_STATE {
        ENTITY_STATE_ACTIVE = 0,
        ENTITY_STATE_INACTIVE = 1,
    };

    struct HB_API EntityState {
        COMPONENT_IDS(EntityState)
        ENTITY_STATE state = ENTITY_STATE_ACTIVE;
    };
}
