#pragma once

#include "Core.h"
#include "Resource.h"

namespace HBE {
	class Shader;

	class ComputeBuffer;

	typedef uint32_t COMPUTE_PIPELINE_FLAG;
	enum COMPUTE_PIPELINE_FLAGS {
		COMPUTE_PIPELINE_FLAG_NONE = 0,
	};
	struct ComputePipelineInfo {
		Shader *compute_shader;
		COMPUTE_PIPELINE_FLAG flags = COMPUTE_PIPELINE_FLAG_NONE;
	};

	class HB_API ComputePipeline : public Resource {
	public:
		virtual bool isDispatchFinished() = 0;

		virtual void dispatch(uint32_t x, uint32_t y, uint32_t z) = 0;
		virtual void wait() = 0;
		virtual ~ComputePipeline(){};
		virtual void setTexture(uint32_t binding, const Texture *texture, uint32_t mip_level = 0) = 0;
		virtual void setTexture(uint32_t binding, const RenderTarget *render_target) = 0;
		virtual void setTexture(const std::string &name, const Texture *texture, uint32_t mip_level = 0) = 0;
		virtual void setTexture(const std::string &name, const RenderTarget *render_target) = 0;
	};
}
