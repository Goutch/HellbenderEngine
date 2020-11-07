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

void Entity::serialize(Serializer *serializer) const {
    serializer->begin("Entity");
    serializer->addField("name",name);
    serializer->addField("position",getPosition());
    serializer->addField("rotation",getRotation());
    serializer->addField("scale",getScale());
    serializer->end();
}













