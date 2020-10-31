#pragma once


#include <string>
#include "Entity.h"
#include "unordered_set"

class Scene {
    std::unordered_set<Entity *> entities;
    std::unordered_set<Component *> update_listeners;
    std::unordered_set<Component *> draw_listeners;
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

    template<class T>
    T *instantiate(std::string name,Entity* parent= nullptr) {
        Entity *e = new Entity(name, parent);
        entities.insert(e);
        e->init();
        return e->attach<T>();
    }
    template<class ComponentType>
    ComponentType *instantiate(Entity* parent= nullptr) {
        Entity *e = new Entity(parent);
        entities.insert(e);
        e->init();
        return e->attach<ComponentType>();
    }

    void destroy(Entity *entity);

    void subscribeDraw(Component *component);

    void unsubscribeDraw(Component *component);

    void subscribeUpdate(Component *component);

    void unsubscribeUpdate(Component *component);
};

