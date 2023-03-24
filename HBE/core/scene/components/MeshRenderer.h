
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/resource/Mesh.h"
#include "core/resource/GraphicPipelineInstance.h"

namespace HBE {
	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		GraphicPipelineInstance *pipeline_instance = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
		bool active = true;
	};
}