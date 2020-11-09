#pragma once

#include <core/resource/Resource.h>
#include "glm/glm.hpp"
#include "Core.h"
using namespace glm;

class Entity;

class HB_API Component :Resource {
    bool subscribed_draw=false;
    bool subscribed_update=false;
protected:
    Entity *entity = nullptr;
public:

    Component() {};

    void attach(Entity *entity) {
        this->entity = entity;
        onAttach();
    }

    virtual void onAttach() {};

    virtual void onDetach();

    virtual void onUpdate(float delta){};
    virtual void subscribeUpdate() final;
    virtual void unsubscribeUpdate() final;

    virtual void onDraw(){};
    virtual void subscribeDraw() final;
    virtual void unsubscribeDraw() final;

    Entity* getEntity() const{return entity;}

    void serialize(Serializer* serializer) const{};
};
