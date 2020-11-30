#pragma once
#include <string>
#include <list>
#include <core/entity/component/Component.h>
#include <core/serialization/Serializable.h>
#include <core/utility/Log.h>
#include <core/entity/component/Transform.h>

#include "Core.h"
#include "vector"
namespace HBE {
    class Scene;
    class HB_API Entity final : public Serializable {
        friend class Scene;

        static unsigned int current_id;
    public:
        Transform *transform = nullptr;
    private:
        unsigned int id;
        std::string name;
        std::list<Component *> components;

        Entity(Entity *parent = nullptr);

        Entity(std::string name, Entity *parent = nullptr);

        Entity(Transform *parent = nullptr);

        Entity(std::string name, Transform *parent = nullptr);

        void destroy();

    public:
        virtual ~Entity() {};

        virtual void init() {};

        void setName(const std::string &name);

        std::list<Component *> getComponents();

        const std::string &getName() const;

        template<class ComponentType>
        ComponentType *attach() {
            ComponentType *component_type_object = new ComponentType();
            Component *component = static_cast<Component *>(component_type_object);
            component->attach(this);
            components.push_back(component);
            return component_type_object;
        }

        template<class ComponentType> void detach(){
            for (Component *component:components) {
                ComponentType *component_type_object = dynamic_cast<ComponentType *>(component);
                if (component_type_object) {
                    components.remove(component);
                    delete component_type_object;
                    break;
                }
            }
        };
        /*template<class ComponentType=Transform>
        void Entity::detach() {
            Log::warning("Detaching transform component is not possible");
        };*/
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

}