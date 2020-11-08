//
// Created by User on 07-Jun.-2020.
//

#include "Scene.h"
#include "core/serialization/Serializer.h"
Scene::Scene() {

}

Scene::Scene(std::string path) {
    //todo:implement scene loading
}

void Scene::init() {
    for (Entity *e:entities) {
        e->init();
    }
}

void Scene::update(float delta) {
    for (Component *c:update_listeners) {
        c->onUpdate(delta);
    }
}

void Scene::draw() {
    for (Component *c:draw_listeners) {
        c->onDraw();
    }
}

void Scene::terminate() {
    for (Entity *e:entities) {
        delete e;
    }
    entities.clear();
}


Entity *Scene::instantiate(std::string name, Entity *parent) {
    Entity *e = new Entity(name, parent);
    entities.push_back(e);
    e->init();
    return e;
}
Entity *Scene::instantiate(Entity *parent) {
    Entity* e =new Entity(parent);
    entities.push_back(e);
    e->init();
    return e;
}


void Scene::destroy(Entity *e) {
    auto it=entities.begin();
    while (it!=entities.end()) {
        if(*it==e)
        {
            break;
        }
        ++it;
    }
    if(it!=entities.end())
    {
        entities.erase(it);
    }
    e->onDestroy();
    delete e;
}

Scene::~Scene() {
    terminate();
}

void Scene::subscribeDraw(Component *component) {
    draw_listeners.emplace(component);
}

void Scene::unsubscribeDraw(Component *component) {
    draw_listeners.erase(component);
}

void Scene::subscribeUpdate(Component *component) {
    update_listeners.emplace(component);
}

void Scene::unsubscribeUpdate(Component *component) {
    update_listeners.erase(component);
}

void Scene::serialize(Serializer *serializer) const {
    serializer->begin("Scene");
    serializer->beginArray("entities");
    for (auto e: entities)
    {
        e->serialize(serializer);
    }
    serializer->endArray();
    serializer->end();
}

const std::vector<Entity *> &Scene::getEntities() {
    return entities;
}









