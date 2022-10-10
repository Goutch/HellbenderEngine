#pragma once

#include "Core.h"
#include "core/resource/IPipelineInstance.h"
#include "string"

namespace HBE {
	class GraphicPipeline;

	typedef uint32_t MATERIAL_FLAGS;
	enum MATERIAL_FLAG {
		MATERIAL_FLAG_NONE = 0,
	};
	struct MaterialInfo {
		GraphicPipeline *graphic_pipeline;
		MATERIAL_FLAGS flags = MATERIAL_FLAG_NONE;
	};

	/**
	 * @brief A Material contains the uniform buffers(descriptorSets) and textures to use in a graphic pipeline.
	 */
	class HB_API GraphicPipelineInstance : public IPipelineInstance {
	public:
		virtual const GraphicPipeline *getGraphicPipeline() const = 0;
	};
}
