#pragma once
#include "string"
#include "list"
#include "Application.h"
class SceneHierarchy {

public:
    static std::unordered_set<Entity*> selected_entities;

    static const char* name;
    static void draw(bool& active);
};
