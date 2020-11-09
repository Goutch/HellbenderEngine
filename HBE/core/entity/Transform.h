//
// Created by User on 2020-01-02.
//

#pragma once
#include <set>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Core.h"
using namespace glm;
class HB_API Transform {

protected:
    quat rotation=glm::identity<quat>();
    mat4 transform_matrix=glm::mat4(1.0f);
public:
    Transform();
    Transform(vec3 position,vec3 rotation,vec3 scale);

    virtual mat4 getMatrix() const;

    virtual vec3 getPosition() const ;
    vec3 getLocalPosition() const;
    void setPosition(const vec3& position);
    void translate(const vec3& translation);

    void rotate(const quat& rotation);
    void rotate(float angle,vec3 axis);

    virtual quat getRotation() const;
    virtual void setRotation(const quat& rotation);
    virtual vec3 getEulerRotation() const;

    void scale(vec3 scale);
    vec3 getScale() const;
    void setScale(vec3 scale);

    vec3 getLocalForward() const;
    vec3 getLocalBackward() const;
    vec3 getLocalRight() const;
    vec3 getLocalLeft() const;
    vec3 getLocalUp() const;
    vec3 getLocalDown() const;
};


