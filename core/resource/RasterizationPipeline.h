#pragma once

#include "Core.h"
#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "Resource.h"

namespace HBE {
	class Shader;

	class Image;

	class MeshLayout;

	class RenderTarget;

	typedef uint32_t RASTERIZATION_PIPELINE_FLAGS;
	enum RASTERIZATION_PIPELINE_FLAG {
		RASTERIZATION_PIPELINE_FLAG_NONE = 0,
		RASTERIZATION_PIPELINE_FLAG_CULL_BACK = 1 << 0,
		RASTERIZATION_PIPELINE_FLAG_CULL_FRONT = 1 << 1,
		RASTERIZATION_PIPELINE_FLAG_NO_DEPTH_TEST = 1 << 2,
		RASTERIZATION_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE = 1 << 3,
		RASTERIZATION_PIPELINE_FLAG_ALLOW_EMPTY_DESCRIPTOR = 1 << 4,
	};

	struct RasterizationPipelineInfo {
		const Shader *vertex_shader = nullptr;
		const Shader *fragment_shader = nullptr;
		const Shader *geometry_shader = nullptr;
		const VertexAttributeInfo *attribute_infos = nullptr;
		const RenderTarget *render_target = nullptr;
		uint32_t attribute_info_count = 0;
		VERTEX_TOPOLOGY topology = VERTEX_TOPOLOGY_TRIANGLE;
		RASTERIZATION_PIPELINE_FLAGS flags = RASTERIZATION_PIPELINE_FLAG_NONE;
	};

	class HB_API RasterizationPipeline : public Resource {
	public:
		virtual ~RasterizationPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void pushConstant(const std::string &name, const void *data) const = 0;

		bool isCompatible(VertexAttributeInfo *attribute_infos, uint32_t attribute_info_count) const;
	};


}
