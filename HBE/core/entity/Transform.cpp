//
// Created by User on 2020-01-02.
//

#include "Transform.h"
#include "Entity.h"

Transform::Transform() {

}

Transform::Transform(vec3 position, vec3 rotation, vec3 scale) {
    this->translate(position);
    this->rotate(quat(rotation));
    this->setScale(scale);
}

mat4 Transform::getMatrix() const {
    return transform_matrix;
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
    return rotation;
}

vec3 Transform::getEulerRotation() const {
    return glm::eulerAngles(rotation) ;
}

void Transform::setRotation(const quat &rotation) {
    this->rotation = glm::identity<quat>();
    vec3 p = getPosition();
    vec3 s = getScale();
    transform_matrix = mat4(1.0f);
    this->translate(p);
    this->rotate(rotation);
    this->setScale(s);
}

vec3 Transform::getPosition() const {
    return transform_matrix[3];
}

vec3 Transform::getLocalPosition() const {
    return transform_matrix[3];
}

vec3 Transform::getScale() const {
    vec3 s;
    s.x = glm::length(transform_matrix[0]);
    s.y = glm::length(transform_matrix[1]);
    s.z = glm::length(transform_matrix[2]);
    return s;
}

void Transform::setPosition(const vec3 &position) {
    transform_matrix[3].x = position.x;
    transform_matrix[3].y = position.y;
    transform_matrix[3].z = position.z;
}

void Transform::scale(vec3 scale) {
    transform_matrix[0] = transform_matrix[0] + (normalize(transform_matrix[0]) * scale.x);
    transform_matrix[1] = transform_matrix[1] + (normalize(transform_matrix[1]) * scale.y);
    transform_matrix[2] = transform_matrix[2] + (normalize(transform_matrix[2]) * scale.z);
}

void Transform::setScale(vec3 scale) {
    transform_matrix[0] = normalize(transform_matrix[0]) * scale.x;
    transform_matrix[1] = normalize(transform_matrix[1]) * scale.y;
    transform_matrix[2] = normalize(transform_matrix[2]) * scale.z;
}

vec3 Transform::getLocalForward() const {
   return glm::normalize(transform_matrix[2]);
}

vec3 Transform::getLocalBackward() const {
    return -glm::normalize(transform_matrix[2]);
}

vec3 Transform::getLocalRight() const {
    return glm::normalize(transform_matrix[0]);
}

vec3 Transform::getLocalLeft() const {
    return -glm::normalize(transform_matrix[0]);
}

vec3 Transform::getLocalUp() const {
    return glm::normalize(transform_matrix[1]);
}

vec3 Transform::getLocalDown() const {
    return -glm::normalize(transform_matrix[1]);
}












