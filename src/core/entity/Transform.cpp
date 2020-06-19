//
// Created by User on 2020-01-02.
//

#include "Transform.h"

Transform::Transform() {

}

Transform::Transform(vec3 position, vec3 rotation, vec3 scale) {
    this->translate(position);
    this->rotate(quat(rotation));
    this->setScale(scale);
}

mat4 Transform::getMatrix() const {
    return parent == nullptr ? transform_matrix : parent->getMatrix() * transform_matrix;
}

void Transform::addChild(Transform *child) {
    if(children.find(child)==children.end())
    {
        children.insert(child);
        child->parent=this;
    }
}

void Transform::removeChild(Transform *child) {
    if(children.find(child)!=children.end())
    {
        children.erase(child);
        child->parent= nullptr;
    }
}

void Transform::setParent(Transform *parent) {
    if(parent!= nullptr)
    {
        parent->addChild(this);
    }
    else
    {
        this->parent= nullptr;
    }

}

const std::set<Transform *>& Transform::getChildren() const{
    return children;
}
const Transform* Transform::getParent() const
{
    return parent;
}
void Transform::translate(const vec3 &translation) {
    transform_matrix = glm::translate(transform_matrix, translation);
}

void Transform::rotate(const quat &rotation) {
    this->rotation *= rotation;
    mat4 rot_matrix = glm::toMat4(rotation);
    transform_matrix = transform_matrix * rot_matrix;
}

void Transform::rotate(float angle, vec3 axis) {
    rotation = glm::rotate(rotation, angle, axis);
    transform_matrix = glm::rotate(transform_matrix, angle, axis);
}

quat Transform::getRotation() const {
    return parent == nullptr ?rotation: rotation * parent->getRotation();
}
vec3 Transform::getEulerRotation() const {
    return glm::eulerAngles(rotation);
}

void Transform::setRotation(const quat &rotation) {
    this->rotation=glm::identity<quat>();
    vec3 p=getPosition();
    vec3 s=getScale();
    transform_matrix=mat4(1.0f);
    this->translate(p);
    this->rotate(rotation);
    this->setScale(s);
}

vec3 Transform::getPosition() const {
    return parent == nullptr ?transform_matrix[3]: (vec3)(parent->getMatrix()*transform_matrix)[3];
}

vec3 Transform::getLocalPosition() const {
    return transform_matrix[3];
}

vec3 Transform::getScale() const{
    vec3 s;
    s.x = glm::length(transform_matrix[0]);
    s.y = glm::length(transform_matrix[1]);
    s.z = glm::length(transform_matrix[2]);
    return s;
}

void Transform::setPosition(const vec3 &position) {
    transform_matrix[3].x= position.x;
    transform_matrix[3].y=position.y;
    transform_matrix[3].z=position.z;
}

void Transform::scale(vec3 scale) {
    transform_matrix[0] = transform_matrix[0] + (normalize(transform_matrix[0]) * scale.x);
    transform_matrix[1] = transform_matrix[1] + (normalize(transform_matrix[1]) *scale.y);
    transform_matrix[2] = transform_matrix[2] + (normalize(transform_matrix[2]) *scale.z);
}

void Transform::setScale(vec3 scale) {
    transform_matrix[0] = normalize(transform_matrix[0]) * scale.x;
    transform_matrix[1] = normalize(transform_matrix[1]) * scale.y;
    transform_matrix[2] = normalize(transform_matrix[2]) * scale.z;
}



vec3 Transform::getForward() const{
    return glm::normalize(transform_matrix[2]);
}
vec3 Transform::getBackward() const{
    return -glm::normalize(transform_matrix[2]);
}

vec3 Transform::getRight()  const{
    return glm::normalize(transform_matrix[0]);
}
vec3 Transform::getLeft()  const{
    return -glm::normalize(transform_matrix[0]);
}

vec3 Transform::getUp()  const{
    return glm::normalize(transform_matrix[1]);
}
vec3 Transform::getDown()  const{
    return -glm::normalize(transform_matrix[1]);
}












