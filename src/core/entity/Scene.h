#pragma once


#include <string>
#include <core/utility/Event.h>
#include "list"
#include "Entity.h"

/*
 * Storage container for entities
 */
class Scene {
    std::set<Entity *> entities;
    std::set<Component*> update_listeners;
    std::set<Component*> draw_listeners;
public:

    Scene(std::string path);

    Scene();

    ~Scene();
    void init();
    void update(float delta);
    void draw();
    void terminate();


    Entity *instantiate(std::string name, Entity *parent = nullptr);
    Entity *instantiate(Entity *parent = nullptr);
    void destroy(Entity *entity);

    void subscribeDraw(Component* component);
    void unsubscribeDraw(Component* component);
    void subscribeUpdate(Component* component);
    void unsubscribeUpdate(Component* component);
};

