#pragma once


#include <string>
#include "list"
#include "Entity.h"

/*
 * Storage container for entities
 */
class Scene {
    std::set<Entity *> entities;
public:
    Scene(std::string path);

    Scene();

    ~Scene();

    void update(float delta);
    void draw();
    void terminate();

    void init();

    void serialize(std::string path);

    Entity *instantiate(std::string name, Entity *parent = nullptr);

    void destroy(Entity *entity);

    Entity *instantiate();
};

