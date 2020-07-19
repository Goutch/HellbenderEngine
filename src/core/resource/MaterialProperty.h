#pragma once
#include "glm/glm.hpp"
#include "Texture.h"

enum GLSL_TYPE{
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    MAT4,
    INT,
};

using namespace glm;

struct MaterialProperty
{
    GLSL_TYPE type;
    void* value;
    MaterialProperty(float v) {
        type=GLSL_TYPE::FLOAT;
        value=new float(v);
    }

    MaterialProperty(int v) {
        type=GLSL_TYPE::INT;
        value=new int(v);
    }

    MaterialProperty(vec2 v) {
        type=GLSL_TYPE::VEC2;
        value=new vec2(v);
    }

    MaterialProperty(vec3 v) {
        type=GLSL_TYPE::VEC3;
        value=new vec3(v);
    }

    MaterialProperty(vec4 v) {
        type=GLSL_TYPE::VEC4;
        value=new vec4(v);
    }

    MaterialProperty(mat4 v) {
        type=GLSL_TYPE::MAT4;
        value=new mat4(v);
    }
};