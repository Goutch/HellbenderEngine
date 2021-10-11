#pragma once
#include "Core.h"
#include "vector"
#include "core/math/Math.h"
namespace HBE
{
    struct HB_API Transform {
        mat4 transform_matrix = mat4(1.0f);
        vec3 scale = vec3(1.0f);
    };

}
