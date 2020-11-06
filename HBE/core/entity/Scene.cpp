//
// Created by User on 07-Jun.-2020.
//

#include "Scene.h"

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
    entities.insert(e);
    e->init();
    return e;
}
Entity *Scene::instantiate(Entity *parent) {
    Entity* e =new Entity(parent);
    entities.insert(e);
    e->init();
    return e;
}


void Scene::destroy(Entity *e) {
    entities.erase(e);
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









