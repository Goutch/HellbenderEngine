#include "VertexLayout.h"
namespace HBE{

    uint32_t VertexLayout::getBytesPerVertex() const {
        uint32_t bytes_per_vertex=0;
        for (size_t i = 0; i < layout.size(); ++i) {
            switch (layout[i]) {
                case GLSL_TYPE::FLOAT:
                case GLSL_TYPE::INT:
                case GLSL_TYPE::UINT:
                    bytes_per_vertex += 4;
                    break;
                case GLSL_TYPE::VEC2F:
                case GLSL_TYPE::VEC2I:
                case GLSL_TYPE::VEC2UI:
                    bytes_per_vertex += 8;
                    break;
                case GLSL_TYPE::VEC3F:
                case GLSL_TYPE::VEC3I:
                case GLSL_TYPE::VEC3UI:
                    bytes_per_vertex += 12;
                    break;
                case GLSL_TYPE::VEC4F:
                case GLSL_TYPE::VEC4I:
                case GLSL_TYPE::VEC4UI:
                    bytes_per_vertex += 16;
                case GLSL_TYPE::MAT3F:
                case GLSL_TYPE::MAT3I:
                case GLSL_TYPE::MAT3UI:
                    bytes_per_vertex += 36;
                    break;
                case GLSL_TYPE::MAT4F:
                case GLSL_TYPE::MAT4I:
                case GLSL_TYPE::MAT4UI:
                    bytes_per_vertex += 64;
                    break;
            }
        }
        return bytes_per_vertex;
    }
}