#pragma once

#include <core/serialization/Serializable.h>

#include "glm/glm.hpp"
#include "Core.h"
#include "string"

using namespace glm;


namespace HBE {
    class Entity;
    class HB_API Component : public Serializable {
        bool subscribed_draw = false;
        bool subscribed_update = false;
    public:
        Entity *entity = nullptr;
    public:

        Component() {};

        virtual ~Component();

        void attach(Entity *entity) {
            this->entity = entity;
            onAttach();
        }

        virtual void onAttach() {};

        virtual void onUpdate(float delta) {};

        virtual void onDraw() {};

        virtual void onDestroy() {};

        virtual void subscribeUpdate() final;

        virtual void unsubscribeUpdate() final;

        virtual void subscribeDraw() final;

        virtual void unsubscribeDraw() final;

        virtual std::string toString() const;

        virtual void serialize(Serializer *serializer) const {};

        virtual void deserialize(Deserializer *deserializer) {};
    };
}