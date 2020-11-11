//
// Created by User on 2020-01-06.
//

#include "Entity.h"
#include <core/utility/Log.h>
#include "core/serialization/Serializer.h"

unsigned int Entity::current_id = 0;

Entity::Entity() {
    setName("Entity_" + std::to_string(current_id++));
}

Entity::Entity(Entity *parent) {
    setName("Entity_" + std::to_string(current_id++));
    setParent(parent);
}

Entity::Entity(std::string name) {
    setName(name);
}

Entity::Entity(std::string name, Entity *parent) {
    setName(name);
    setParent(parent);
}

Entity::Entity(vec3 position, vec3 rotation, vec3 scale) : Transform(position, rotation, scale) {
    setName("Entity_" + std::to_string(current_id++));
}

Entity::Entity(std::string name, vec3 position, vec3 rotation, vec3 scale) : Transform(position, rotation, scale) {
    setName(name);
}

Entity::Entity(std::string name, Entity *parent, vec3 position, vec3 rotation, vec3 scale) : Transform(position,
                                                                                                       rotation,
                                                                                                       scale) {
    setName(name);
    setParent(parent);
}

const std::string &Entity::getName() const {
    return name;
}

void Entity::setName(const std::string &name) {
    this->name = name;
}

void Entity::onDestroy() {
    for (Component *component:components) {
        delete component;
    }
    if (parent != nullptr)parent->removeChild(this);
}

void Entity::addChild(Entity *child) {

    if (std::find(children.begin(),children.end(),child) == children.end()) {
        children.push_back(child);
        child->parent = this;
    }
}

void Entity::removeChild(Entity *child) {
    auto it=std::find(children.begin(),children.end(),child);
    if (it != children.end()) {
        children.erase(it);
        child->parent = nullptr;
    }
}

void Entity::setParent(Entity *parent) {
    if (parent != nullptr) {
        parent->addChild(this);
    } else {
        this->parent = nullptr;
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
    serializer->beginArray("Components");
    for (auto c : components) {
        c->serialize(serializer);
    }
    serializer->endArray();
    serializer->end();
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















