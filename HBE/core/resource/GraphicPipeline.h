#pragma once

#include "Core.h"
#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "Resource.h"

namespace HBE {
	class Shader;

	class Texture;

	class MeshLayout;

	class RenderTarget;

	typedef uint32_t GRAPHIC_PIPELINE_FLAGS;
	enum GRAPHIC_PIPELINE_FLAG {
		GRAPHIC_PIPELINE_FLAG_NONE = 0,
		GRAPHIC_PIPELINE_FLAG_CULL_BACK = 1 << 0,
		GRAPHIC_PIPELINE_FLAG_CULL_FRONT = 1 << 1,
		GRAPHIC_PIPELINE_FLAG_IGNORE_DEPTH_TEST = 1 << 2,
		GRAPHIC_PIPELINE_FLAG_FRONT_COUNTER_CLOCKWISE = 1 << 3,
	};

	struct GraphicPipelineInfo {
		const Shader *vertex_shader = nullptr;
		const Shader *fragement_shader = nullptr;
		const Shader *geometry_shader = nullptr;
		const VertexAttributeInfo *attribute_infos = nullptr;
		const RenderTarget *render_target = nullptr;
		uint32_t attribute_info_count = 0;
		VERTEX_TOPOLOGY topology = VERTEX_TOPOLOGY_TRIANGLE;
		GRAPHIC_PIPELINE_FLAGS flags = GRAPHIC_PIPELINE_FLAG_NONE;
	};

	class HB_API GraphicPipeline : public Resource {
	public:
		virtual ~GraphicPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void pushConstant(const std::string &name, const void *data) const = 0;
	};


}
