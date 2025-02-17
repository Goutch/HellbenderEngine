#pragma once

#include "Core.h"
#include "core/resource/IPipelineInstance.h"
#include "string"

namespace HBE {
	class RasterizationPipeline;

	typedef uint32_t GRAPHIC_PIPELINE_INSTANCE_FLAGS;
	enum GRAPHIC_PIPELINE_INSTANCE_FLAG {
		GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE = 0,
		GRAPHIC_PIPELINE_INSTANCE_FLAG_EMPTY_DESCRIPTOR_ALLOWED = 1,
	};
	struct RasterizationPipelineInstanceInfo {
		RasterizationPipeline *rasterization_pipeline;
		GRAPHIC_PIPELINE_INSTANCE_FLAGS flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
	};

	class HB_API RasterizationPipelineInstance : public IPipelineInstance {
	public:
		virtual const RasterizationPipeline *getGraphicPipeline() const = 0;
	};
}
