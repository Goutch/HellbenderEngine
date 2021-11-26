#pragma once

#include "Core.h"
#include "vector"
#include "GLSL_TYPE.h"
#include "Resource.h"

namespace HBE {
	struct MeshLayoutInfo {
		uint32_t vertex_binding = 0;
		const GLSL_TYPE *vertex_layout_types = nullptr;
		uint32_t vertex_layout_types_count = 0;

		uint32_t instance_binding = 1;
		const GLSL_TYPE *instance_layout_types = nullptr;
		uint32_t instance_layout_types_count = 0;
	};

	class HB_API MeshLayout : public Resource {
	protected:
		std::vector<GLSL_TYPE> instanced_layout;
		std::vector<GLSL_TYPE> vertex_layout;
	private:

		uint32_t getBytes(const std::vector<GLSL_TYPE> &types) const;

	public:
		virtual ~MeshLayout() = default;

		uint32_t getBytesPerVertex() const;
		uint32_t getBytesPerInstance() const;
	};

}