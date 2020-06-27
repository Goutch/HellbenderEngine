#pragma once

#include "glm/glm.hpp"

using namespace glm;

class Entity;

class Component {
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

    Entity* getEntity(){return entity;}
};
