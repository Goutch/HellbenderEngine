#pragma once
#include "Core.h"
#include "glm/glm.hpp"

namespace HBE {
    struct HB_API Plane {
        glm::vec3 normal;
        float d;

        Plane() {}

        Plane(const glm::vec4 &equation) {
            set(equation);
        }

        void set(const glm::vec4 &equation) {
            this->normal = glm::vec3(equation.x, equation.y, equation.z);
            float l = glm::length(normal);
            normal /= l;
            this->d = equation.w / l;
        }

        float distance(const glm::vec3 &point) const {
            return (normal.x * point.x) + (normal.y * point.y) + (normal.z * point.z) + d;
        }
    };
}
