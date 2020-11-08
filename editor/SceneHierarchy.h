#pragma once
#include "string"
#include "list"
#include "HBE.h"
class SceneHierarchy {

public:
    static std::list<Entity*> selected_entities;
    static const char* name;
    static void draw(bool& active);
};
