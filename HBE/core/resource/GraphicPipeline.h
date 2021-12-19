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
		GRAPHIC_PIPELINE_FLAG_NO_DEPTH_TEST = 1 << 2,
	};

	typedef uint32_t VERTEX_BINDING_FLAGS;
	enum VERTEX_BINDING_FLAG {
		VERTEX_BINDING_FLAG_NONE = 0,
		VERTEX_BINDING_FLAG_PER_INSTANCE = 1 << 0,
		/// Disable device local memory and skip staging buffer. May result in slower read in shader.
		VERTEX_BINDING_FLAG_FAST_WRITE = 1 << 1,
		///Recommended for buffer changing every frame, dont wait for rendering to finish but double or triple memory usage.
		VERTEX_BINDING_FLAG_MULTIPLE_BUFFERS = 1 << 2,
	};
	struct VertexBindingInfo {
		uint32_t binding = 0;
		uint32_t size = 0;
		VERTEX_BINDING_FLAGS flags = VERTEX_BINDING_FLAG_NONE;
	};
	struct GraphicPipelineInfo {
		const Shader *vertex_shader = nullptr;
		const Shader *fragement_shader = nullptr;
		const Shader *geometry_shader = nullptr;
		const VertexBindingInfo *binding_infos = nullptr;
		uint32_t binding_info_count = 0;
		GRAPHIC_PIPELINE_FLAGS flags = GRAPHIC_PIPELINE_FLAG_NONE;
	};

	class HB_API GraphicPipeline : public Resource {
	public:
		virtual ~GraphicPipeline() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;

		virtual void setDynamicUniform(const std::string &name, const void *data) = 0;
		virtual void setDynamicUniform(uint32_t binding, const void *data) = 0;

		virtual void setUniform(const std::string &name, const void *data) = 0;
		virtual void setUniform(uint32_t binding, const void *data) = 0;
		virtual void pushConstant(const std::string &name, const void *data) = 0;
		virtual void setTexture(uint32_t binding, const Texture *texture) = 0;
		virtual void setTexture(uint32_t binding, const RenderTarget *render_target) = 0;
		virtual void setTexture(const std::string &name, const Texture *texture) = 0;
		virtual void setTexture(const std::string &name, const RenderTarget *render_target) = 0;
	};


}
