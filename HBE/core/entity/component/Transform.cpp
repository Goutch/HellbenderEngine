//
// Created by User on 2020-01-02.
//

#include "Transform.h"
#include "core/entity/Entity.h"
#include "algorithm"
#include "Application.h"

namespace HBE {
    void Transform::onDestroy() {
        if (parent != nullptr)
            parent->removeChild(this);
        for (auto t : children) {
            Application::scene->destroy(t->entity);
        }
        Component::onDestroy();
    }

    const std::vector<Transform *> &Transform::getChildren() const {
        return children;
    }

    const Transform *Transform::getParent() const {
        return parent;
    }

    const mat4 &Transform::getMatrix() const {
        return transform_matrix;
    }

    mat4 Transform::getWorldMatrix() const {
        return parent == nullptr ? transform_matrix : parent->getMatrix() * transform_matrix;
    }

    vec3 Transform::getWorldPosition() const {
        return parent == nullptr ? transform_matrix[3] : (vec3) (parent->getMatrix() * transform_matrix)[3];
    }

    quat Transform::getWorldRotation() const {
        return parent == nullptr ? getRotation() : getRotation() * parent->getWorldRotation();
    }

    vec3 Transform::getWorldEulerRotation() const {
        return parent == nullptr ? glm::eulerAngles(getRotation()) : glm::eulerAngles(toQuat(transform_matrix) * parent->getWorldRotation());
    }

    void Transform::translate(const vec3 &translation) {
        transform_matrix = glm::translate(transform_matrix, translation);
    }

    void Transform::rotate(const quat &rotation) {
        mat4 rot_matrix = glm::toMat4(rotation);
        transform_matrix = transform_matrix * rot_matrix;
    }

    void Transform::rotate(float angle, vec3 axis) {
        transform_matrix = glm::rotate(transform_matrix, angle, axis);
    }

    quat Transform::getRotation() const {
        return glm::quat_cast(transform_matrix);
    }

    vec3 Transform::getEulerRotation() const {
        return glm::eulerAngles(toQuat(transform_matrix));
    }

    void Transform::setRotation(const quat &rotation) {
        vec3 p = getPosition();
        transform_matrix = mat4(1.0f);
        this->translate(p);
        this->rotate(rotation);
        this->setScale(scale);
    }

    vec3 Transform::getPosition() const {
        return transform_matrix[3];
    }

    vec3 Transform::getScale() const {
        return scale;
    }

    void Transform::setPosition(const vec3 &position) {
        transform_matrix[3].x = position.x;
        transform_matrix[3].y = position.y;
        transform_matrix[3].z = position.z;
    }

    void Transform::setScale(vec3 scale) {
        this->scale = scale;
        vec3 position = getPosition();
        setPosition(vec3(0));
        transform_matrix[0] = abs(normalize(transform_matrix[0])) * scale.x;
        transform_matrix[1] = abs(normalize(transform_matrix[1])) * scale.y;
        transform_matrix[2] = abs(normalize(transform_matrix[2])) * scale.z;
        setPosition(position);
    }

    vec3 Transform::getForward() const {
        return glm::normalize(transform_matrix[2]);
    }

    vec3 Transform::getBackward() const {
        return -glm::normalize(transform_matrix[2]);
    }

    vec3 Transform::getRight() const {
        return glm::normalize(transform_matrix[0]);
    }

    vec3 Transform::getLeft() const {
        return -glm::normalize(transform_matrix[0]);
    }

    vec3 Transform::getUp() const {
        return glm::normalize(transform_matrix[1]);
    }

    vec3 Transform::getDown() const {
        return -glm::normalize(transform_matrix[1]);
    }

    vec3 Transform::getWorldForward() const {
        return glm::normalize(getWorldMatrix()[2]);
    }

    vec3 Transform::getWorldBackward() const {
        return -glm::normalize(getWorldMatrix()[2]);
    }

    vec3 Transform::getWorldRight() const {
        return glm::normalize(getWorldMatrix()[0]);
    }

    vec3 Transform::getWorldLeft() const {
        return -glm::normalize(getWorldMatrix()[0]);
    }

    vec3 Transform::getWorldUp() const {
        return glm::normalize(getWorldMatrix()[1]);
    }

    vec3 Transform::getWorldDown() const {
        return -glm::normalize(getWorldMatrix()[1]);

    }

    void Transform::addChild(Transform *child) {
        if (std::find(children.begin(), children.end(), child) == children.end()) {
            children.push_back(child);
            child->parent = this;
        }
    }

    void Transform::removeChild(Transform *child) {
        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
            child->parent = nullptr;
        }
    }

    void Transform::setParent(Transform *parent) {
        if (parent != nullptr) {
            parent->addChild(this);
        }
    }

    std::string Transform::toString() const {
        return "Transform";
    }
}




