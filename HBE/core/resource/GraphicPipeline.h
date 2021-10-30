#pragma once

#include "Core.h"
#include "string"
#include "vector"
#include "glm/glm.hpp"
#include "core/graphics/DrawFlags.h"
#include "Resource.h"

namespace HBE {
	class Shader;

	class Texture;

	class VertexLayout;

	enum GRAPHIC_PIPELINE_FLAGS {
		GRAPHIC_PIPELINE_FLAG_NONE = 0,
		GRAPHIC_PIPELINE_FLAG_DEPTH_TEST = 1
	};

	struct GraphicPipelineInfo {
		const VertexLayout *vertex_layout = nullptr;
		const Shader *vertex_shader = nullptr;
		const Shader *fragement_shader = nullptr;
		const Shader *geometry_shader = nullptr;
		GRAPHIC_PIPELINE_FLAGS flags = GRAPHIC_PIPELINE_FLAG_NONE;
	};

	class HB_API GraphicPipeline : public Resource {
	public:
		virtual ~GraphicPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void setDrawFlags(DRAW_FLAGS flags) = 0;

		virtual DRAW_FLAGS getDrawFlags() const = 0;


		virtual void setDynamicUniform(const std::string &name, void *data) = 0;
		virtual void setDynamicUniform(uint32_t binding, void *data) = 0;

		virtual void setUniform(const std::string &name, void *data) = 0;
		virtual void setUniform(uint32_t binding, void *data) = 0;
		virtual void pushConstant(const std::string &name, void *data) = 0;
		virtual void setTexture(uint32_t binding, const Texture *texture) = 0;
		virtual void setTexture(const std::string &name, const Texture *texture) = 0;
	};


}
