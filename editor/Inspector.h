#pragma once

#include <list>
#include "HBE.h"

using namespace HBE;

class Inspector {
public:
    static void draw(bool active);

    static const char *name;
    static std::list<Component *> components_to_detach;
};

