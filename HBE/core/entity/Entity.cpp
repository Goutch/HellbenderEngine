//
// Created by User on 2020-01-06.
//

#include "Entity.h"
#include <Application.h>
#include "core/serialization/Serializer.h"
#include "core/serialization/Deserializer.h"
#include "core/serialization/ComponentRegistry.h"

namespace HBE {
    unsigned int Entity::current_id = 0;

    Entity::Entity(Entity *parent) {
        setName("Entity_" + std::to_string(current_id++));
        transform = attach<Transform>();
        if (parent != nullptr)
            transform->setParent(parent->transform);
    }

    Entity::Entity(Transform *parent) {
        setName("Entity_" + std::to_string(current_id++));
        transform = attach<Transform>();
        if (parent != nullptr)
            transform->setParent(parent);
    }


    Entity::Entity(std::string name, Entity *parent) {
        setName(name);
        transform = attach<Transform>();
        if (parent != nullptr)
            transform->setParent(parent->transform);
    }

    Entity::Entity(std::string name, Transform *parent) {
        setName(name);
        transform = attach<Transform>();
        if (parent != nullptr)
            transform->setParent(parent);
    }

    void Entity::destroy() {
        for (Component *component:components) {
            component->onDetach();
        }
        for (Component *component:components) {
            delete component;
        }
    }

    const std::string &Entity::getName() const {
        return name;
    }

    void Entity::setName(const std::string &name) {
        this->name = name;
    }

    void Entity::serialize(Serializer *serializer) const {
        serializer->begin("Entity");
        serializer->addField("name", name);
        serializer->begin("Components");
        for (auto c : components) {
            c->serialize(serializer);
        }
        serializer->end();
        serializer->end();
    }

    void Entity::deserialize(Deserializer *deserializer) {
        name = deserializer->getString("name");
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
            Entity *c = Application::scene->instantiate(this);
            c->deserialize(deserializer);
        }
        deserializer->popArray();
    }


    std::list<Component *> Entity::getComponents() {
        return components;
    }

    void Entity::detach(Component *component) {
        if(component->toString()!="Transform")
        {
            for (Component *c :components) {
                if (c == component) {
                    components.remove(c);
                    c->onDetach();
                    delete c;
                    break;
                }
            }
        } else{
            Log::warning("Detaching transform component is not permitted");
        }
    }

    template<>
    void Entity::detach<Transform>() {
        Log::warning("Detaching transform component is not permitted");
    };
}

















