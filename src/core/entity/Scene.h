#pragma once


#include <string>
#include "Entity.h"
#include "unordered_set"
/*
 * Storage container for entities
 */
class Scene {
    std::unordered_set<Entity *> entities;
    std::unordered_set<Component*> update_listeners;
    std::unordered_set<Component*> draw_listeners;
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

