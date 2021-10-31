#pragma once

#include "Core.h"
#include "vector"
#include "GLSL_TYPE.h"
#include "Resource.h"

namespace HBE {
    struct VertexLayoutInfo {
        const GLSL_TYPE *layout_types = nullptr;
        uint32_t layout_types_count = 0;
    };

    class HB_API VertexLayout : public Resource {
    protected:
        std::vector<GLSL_TYPE> layout;
    public:
        virtual ~VertexLayout() = default;

        uint32_t getBytesPerVertex() const;
    };

}