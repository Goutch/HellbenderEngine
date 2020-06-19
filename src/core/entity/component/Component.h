#pragma once

#include "glm/glm.hpp"

using namespace glm;

class Entity;

class Component {
protected:
    Entity *entity= nullptr;
public:
    Component() {};

    void attach(Entity *entity) {
        this->entity = entity;
        onAttach();
    }

    virtual void onAttach() {};

    virtual void onDraw() {}

    virtual void onUpdate(float delta) {}

    virtual void onDetach() {}
};
