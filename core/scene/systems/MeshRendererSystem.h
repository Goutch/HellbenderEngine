#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"

namespace HBE
{
	struct PushConstantInfo;
	class Scene;

	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	struct HB_API MeshRenderer
	{
		Mesh* mesh = nullptr;
		RasterizationPipelineInstance* pipeline_instance = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
		bool active = true;
		bool use_transform_matrix_as_push_constant = true;
		PushConstantInfo* push_constants = nullptr;
		uint32_t push_constants_count = 0;
	};

	class MeshRendererSystem : public System
	{
	public:
		MeshRendererSystem(Scene* scene);

		~MeshRendererSystem();

		void onDraw(RenderGraph* render_graph);
	};
}
