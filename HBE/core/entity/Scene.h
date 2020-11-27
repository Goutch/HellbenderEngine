#pragma once


#include <string>
#include "Entity.h"
#include "unordered_set"
#include "Core.h"

class HB_API Scene : public Serializable {
    friend class Entity;

    std::vector<Entity *> scene_tree;
    std::unordered_set<Entity *> entities;

    std::unordered_set<Component *> update_listeners;
    std::unordered_set<Component *> draw_listeners;
public:

    Scene(std::string path);

    Scene();

    virtual ~Scene();

    void init();

    void update(float delta);

    void draw();

    void terminate();

    const std::vector<Entity *> &getSceneTree();

    Entity *instantiate(std::string name, Entity *parent = nullptr);

    Entity *instantiate(Entity *parent = nullptr);

    template<class ComponentType>
    ComponentType *instantiate(std::string name, Entity *parent = nullptr) {
        Entity *e = new Entity(this, name, parent);
        entities.emplace(e);
        e->init();
        return e->attach<ComponentType>();
    }

    template<class ComponentType>
    ComponentType *instantiate(Entity *parent = nullptr) {
        Entity *e = new Entity(this, parent);
        entities.emplace(e);
        e->init();
        return e->attach<ComponentType>();
    }

    void destroy(Entity *entity);

    void subscribeDraw(Component *component);

    void unsubscribeDraw(Component *component);

    void subscribeUpdate(Component *component);

    void unsubscribeUpdate(Component *component);

    void serialize(Serializer *serializer) const override;

    void deserialize(Deserializer *deserializer) override;
};

