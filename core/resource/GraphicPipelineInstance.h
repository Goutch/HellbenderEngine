#pragma once

#include "Core.h"
#include "core/resource/IPipelineInstance.h"
#include "string"

namespace HBE {
	class GraphicPipeline;

	typedef uint32_t GRAPHIC_PIPELINE_INSTANCE_FLAGS;
	enum GRAPHIC_PIPELINE_INSTANCE_FLAG {
		GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE = 0,
	};
	struct GraphicPipelineInstanceInfo {
		GraphicPipeline *graphic_pipeline;
		GRAPHIC_PIPELINE_INSTANCE_FLAGS flags = GRAPHIC_PIPELINE_INSTANCE_FLAG_NONE;
	};

	/**
	 * @brief A Material contains the uniform buffers(descriptorSets) and textures to use in a graphic pipeline.
	 */
	class HB_API GraphicPipelineInstance : public IPipelineInstance {
	public:
		virtual const GraphicPipeline *getGraphicPipeline() const = 0;
	};
}
