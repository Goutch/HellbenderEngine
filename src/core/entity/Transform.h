//
// Created by User on 2020-01-02.
//

#pragma once
#include <set>
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"
using namespace glm;
class Transform {
private:
    quat rotation=glm::identity<quat>();
    mat4 transform_matrix=glm::mat4(1.0f);
protected:
    std::set<Transform*> children;
    Transform* parent= nullptr;
public:
    Transform();
    Transform(vec3 position,vec3 rotation,vec3 scale);

    virtual mat4 getMatrix() const;

    vec3 getPosition() const ;
    vec3 getLocalPosition() const;
    void setPosition(const vec3& position);
    void translate(const vec3& translation);

    void rotate(const quat& rotation);
    void rotate(float angle,vec3 axis);

    quat getRotation() const;
    void setRotation(const quat& rotation);
    vec3 getEulerRotation() const;

    void scale(vec3 scale);
    vec3 getScale() const;
    void setScale(vec3 scale);

    vec3 getForward() const;
    vec3 getBackward() const;
    vec3 getRight() const;
    vec3 getLeft() const;
    vec3 getUp() const;
    vec3 getDown() const;

    void addChild(Transform* child);
    void removeChild(Transform* child);
    const std::set<Transform*>& getChildren() const;
    void setParent(Transform* parent);
    const Transform* getParent() const;

};


