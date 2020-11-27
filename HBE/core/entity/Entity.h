#pragma once

#include "Transform.h"
#include <string>
#include <list>
#include <core/entity/component/Component.h>
#include <core/serialization/Serializable.h>

#include "Core.h"
#include "vector"

class Scene;

class HB_API Entity : public Transform, public Serializable {
    friend class Scene;

    Scene *scene;
    static unsigned int current_id;

    std::vector<Entity *> children;
    Entity *parent = nullptr;
    unsigned int id;
    std::string name;
    std::list<Component *> components;

    Entity(Scene *scene, Entity *parent = nullptr);

    Entity(Scene *scene, std::string name, Entity *parent = nullptr);


    void destroy();
    void removeChild(Entity *child);
public:

    virtual void init() {};

    void setName(const std::string &name);

    std::list<Component *> getComponents();

    const std::string &getName() const;

    void addChild(Entity *child);

    const std::vector<Entity *> &getChildren() const;

    void setParent(Entity *parent);

    const Entity *getParent() const;

    mat4 getMatrix() const override;

    vec3 getPosition() const override;

    quat getRotation() const override;

    vec3 getEulerRotation() const override;

    vec3 getForward() const;

    vec3 getBackward() const;

    vec3 getRight() const;

    vec3 getLeft() const;

    vec3 getUp() const;

    vec3 getDown() const;

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
            if (component_type_object) {
                components.remove(component);
                delete component_type_object;
                break;
            }
        }
    }

    template<class ComponentType>
    ComponentType *getComponent() {
        for (Component *component:components) {
            ComponentType *component_type = dynamic_cast<ComponentType *>(component);
            if (component_type)
                return component_type;
        }
        return nullptr;
    }

    void serialize(Serializer *serializer) const override;

    void deserialize(Deserializer *deserializer) override;
};