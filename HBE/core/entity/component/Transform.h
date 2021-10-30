#pragma once
#include "Core.h"
#include <set>
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Component.h"
#include "vector"

namespace HBE {
    class HB_API Transform : public Component {
        glm::mat4 transform_matrix = glm::mat4(1.0f);
        glm::vec3 scale = glm::vec3(1.0f);

        std::vector<Transform *> children;
        Transform *parent = nullptr;
    public:
        void onDetach() override;

        std::string toString() const override;

        const mat4 &getMatrix() const;

        mat4 getWorldMatrix() const;

        vec3 getPosition() const;

        vec3 getWorldPosition() const;

        void setPosition(const vec3 &position);

        void translate(const vec3 &translation);

        void rotate(const quat &rotation);

        void rotate(float angle, vec3 axis);

        quat getRotation() const;

        quat getWorldRotation() const;

        vec3 getEulerRotation() const;

        vec3 getWorldEulerRotation() const;

        void setRotation(const quat &rotation);

        glm::vec3 getScale() const;

        void setScale(vec3 scale);

        vec3 getForward() const;

        vec3 getBackward() const;

        vec3 getRight() const;

        vec3 getLeft() const;

        vec3 getUp() const;

        vec3 getDown() const;

        vec3 getWorldForward() const;

        vec3 getWorldBackward() const;

        vec3 getWorldRight() const;

        vec3 getWorldLeft() const;

        vec3 getWorldUp() const;

        vec3 getWorldDown() const;

        const Transform *getParent() const;

        void setParent(Transform *parent);

        void addChild(Transform *child);

        void removeChild(Transform *child);

        const std::vector<Transform *> &getChildren() const;
    };
}

