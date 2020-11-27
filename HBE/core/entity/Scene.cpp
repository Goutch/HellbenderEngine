#include "Scene.h"
#include "core/serialization/Serializer.h"
#include "core/serialization/Deserializer.h"
#include "core/utility/Log.h"
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
    Entity *e = new Entity(this, name, parent);
    entities.emplace(e);
    e->init();
    return e;
}

Entity *Scene::instantiate(Entity *parent) {
    Entity *e = new Entity(this, parent);
    entities.emplace(e);
    e->init();
    return e;
}


void Scene::destroy(Entity *e) {
    Log::debug("destroy "+e->getName());
    entities.erase(entities.find(e));
    if(e->parent== nullptr)
    {
        auto it = std::find(scene_tree.begin(), scene_tree.end(), e);
        if (it != scene_tree.end()) {
            scene_tree.erase(it);
        }
    }
    else
    {
        e->parent->removeChild(e);

    }
    e->destroy();
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
    for (auto e: entities) {
        e->serialize(serializer);
    }
    serializer->endArray();
    serializer->end();
}

void Scene::deserialize(Deserializer *deserializer) {

}

const std::vector<Entity *> &Scene::getSceneTree() {
    return scene_tree;
}











