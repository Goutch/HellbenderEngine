#pragma once

#include "Core.h"
#include "vector"
#include "GLSL_TYPE.h"
#include "Resource.h"

namespace HBE {
	struct VertexLayoutInfo
	{
		const std::vector<GLSL_TYPE>& layout_types;
	};
    class HB_API VertexLayout : public Resource {
    public:
        virtual ~VertexLayout() = default;

        virtual int getBytesPerVertex() const=0;
    };
}