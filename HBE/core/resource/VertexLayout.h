#pragma once

#include "Core.h"
#include "vector"
#include "GLSL_TYPE.h"
#include "Resource.h"

namespace HBE {
    class HB_API VertexLayout : public Resource {
    public:
        virtual ~VertexLayout() = default;

        virtual void setLayoutTypes(std::vector<GLSL_TYPE> layout) = 0;

        virtual int getBytesPerVertex()=0;
    };
}