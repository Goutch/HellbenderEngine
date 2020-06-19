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
    for (Entity *e:entities) {
        e->update(delta);
    }
}

void Scene::draw() {
    for (Entity *e:entities) {
        e->draw();
    }
}

void Scene::terminate() {
    for (Entity *e:entities) {
        delete e;
    }
    entities.clear();
}

void Scene::serialize(std::string path) {
    //todo: implement serialization
}

Entity *Scene::instantiate(std::string name, Entity *parent) {
    Entity *e = new Entity(name, parent);
    entities.emplace(e);
    e->init();
    return e;
}
Entity *Scene::instantiate() {
    Entity* e =new Entity();
    entities.emplace(e);
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







