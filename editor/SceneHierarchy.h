#pragma once
#include "string"
#include "list"
#include "Application.h"
class SceneHierarchy {
    static std::list<Entity*> delete_query;
public:
    static std::unordered_set<Entity*> selected_entities;

    static const char* name;
    static void update();
    static void draw(bool& active);
    static void drawTree(Entity *e);
};
