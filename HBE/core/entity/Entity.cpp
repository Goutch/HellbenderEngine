//
// Created by User on 2020-01-06.
//

#include "Entity.h"
#include <Application.h>
#include "core/serialization/Serializer.h"
#include "core/serialization/Deserializer.h"
#include "core/serialization/ComponentRegistry.h"

unsigned int Entity::current_id = 0;

Entity::Entity(Scene *scene, Entity *parent) {
    this->scene = scene;
    setName("Entity_" + std::to_string(current_id++));
    scene->scene_tree.emplace_back(this);
    if (parent != nullptr)
        setParent(parent);
}


Entity::Entity(Scene *scene, std::string name, Entity *parent) {
    this->scene = scene;
    setName(name);
    scene->scene_tree.emplace_back(this);
    if (parent != nullptr)
        setParent(parent);
}


void Entity::destroy() {
    for (Entity *c :children) {
        Application::scene->destroy(c);
    }
    children.clear();
    for (Component *component:components) {
        delete component;
    }
    if (parent != nullptr)parent->removeChild(this);
}

const std::string &Entity::getName() const {
    return name;
}

void Entity::setName(const std::string &name) {
    this->name = name;
}

void Entity::addChild(Entity *child) {
    if (std::find(children.begin(), children.end(), child) == children.end()) {
        children.push_back(child);
        child->parent = this;
    }
}

void Entity::removeChild(Entity *child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        children.erase(it);
        child->parent = nullptr;
    }
}

void Entity::setParent(Entity *parent) {
    if (this->parent == nullptr) {
        scene->scene_tree.erase(std::find(scene->scene_tree.begin(), scene->scene_tree.end(), this));
    } else {
        this->parent->removeChild(this);
    }
    if (parent == nullptr) {
        scene->scene_tree.push_back(this);
    } else {
        parent->addChild(this);
    }
}

const std::vector<Entity *> &Entity::getChildren() const {
    return children;
}

const Entity *Entity::getParent() const {
    return parent;
}

mat4 Entity::getMatrix() const {
    return parent == nullptr ? transform_matrix : parent->getMatrix() * transform_matrix;
}

vec3 Entity::getPosition() const {
    return parent == nullptr ? transform_matrix[3] : (vec3) (parent->getMatrix() * transform_matrix)[3];
}

quat Entity::getRotation() const {
    return parent == nullptr ? rotation : rotation * parent->getRotation();
}

vec3 Entity::getEulerRotation() const {
    return parent == nullptr ? glm::eulerAngles(rotation) : glm::eulerAngles(rotation * parent->getRotation());
}

void Entity::serialize(Serializer *serializer) const {
    serializer->begin("Entity");
    serializer->addField("name", name);
    serializer->addField("position", getPosition());
    serializer->addField("rotation", getRotation());
    serializer->addField("scale", getScale());
    serializer->begin("Components");
    for (auto c : components) {
        c->serialize(serializer);
    }
    serializer->end();
    serializer->beginArray("Children");
    for (auto c:children) {
        c->serialize(serializer);
    }
    serializer->endArray();
    serializer->end();
}

void Entity::deserialize(Deserializer *deserializer) {
    name = deserializer->getString("name");
    setPosition(deserializer->getVec3("position"));
    vec4 rotation_vec4 = deserializer->getVec4("rotation");
    quat rotation_quat = quat();
    rotation_quat.x = rotation_vec4.x;
    rotation_quat.y = rotation_vec4.y;
    rotation_quat.x = rotation_vec4.z;
    rotation_quat.y = rotation_vec4.w;
    setRotation(rotation_quat);
    setScale(deserializer->getVec3("scale"));

    deserializer->getObject("Components");
    for (int i = 0; i < deserializer->getSize(); ++i) {
        std::string component_name = deserializer->getNext();
        deserializer->getObject(component_name);
        Component *c = ComponentRegistry::attach(component_name, this);
        c->deserialize(deserializer);
        deserializer->popObject();
    }
    deserializer->popObject();

    deserializer->getArray("Children");
    for (int i = 0; i < deserializer->getSize(); ++i) {
        Entity *c = scene->instantiate(this);
        c->deserialize(deserializer);
    }
    deserializer->popArray();
}

vec3 Entity::getForward() const {
    return glm::normalize(getMatrix()[2]);
}

vec3 Entity::getBackward() const {
    return -glm::normalize(getMatrix()[2]);
}

vec3 Entity::getRight() const {
    return glm::normalize(getMatrix()[0]);
}

vec3 Entity::getLeft() const {
    return -glm::normalize(getMatrix()[0]);
}

vec3 Entity::getUp() const {
    return glm::normalize(getMatrix()[1]);
}

vec3 Entity::getDown() const {
    return -glm::normalize(getMatrix()[1]);

}

std::list<Component *> Entity::getComponents() {
    return components;
}




















