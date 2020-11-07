#pragma once

#include "Transform.h"
#include <string>
#include <list>
#include <core/entity/component/Component.h>
#include <core/resource/Resource.h>


class Entity : public Transform , public Resource{
    friend class Scene;
    static unsigned int current_id;
    unsigned int id;
    std::string name;
    std::list<Component *> components;


    Entity(Entity* parent);

    Entity(std::string name);

    Entity(std::string name, Entity *parent);

    Entity(std::string name, vec3 position, vec3 rotation = vec3(0), vec3 scale = vec3(1));

    Entity(std::string name, Entity *parent, vec3 position, vec3 rotation = vec3(0), vec3 scale = vec3(1));

    Entity(vec3 position, vec3 rotation = vec3(0), vec3 scale = vec3(1));

    Entity();

public:
    virtual void init() {};

    virtual void onDestroy();

    void setName(const std::string &name);

    const std::string &getName() const;

    template<class ComponentType>
    ComponentType *attach() {
        ComponentType *component_type_object = new ComponentType();
        Component *component = static_cast<Component *>(component_type_object);
        component->attach(this);
        components.push_back(component);
        return component_type_object;
    }

    template<class ComponentType>
    void detach() {
        for (Component *component:components) {
            ComponentType *component_type_object = dynamic_cast<ComponentType *>(component);
            if (component_type_object)
            {
                components.remove(component);
                component->onDetach();
                break;
            }
        }
    }

    template<class ComponentType>
    ComponentType *getComponent() {
        for (Component *component:components) {
            ComponentType *componentType = dynamic_cast<ComponentType *>(component);
            if (componentType)
                return componentType;
        }
        return nullptr;
    }

    void serialize(Serializer* serializer) const override;
};