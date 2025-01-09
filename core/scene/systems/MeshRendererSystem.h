
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"

namespace HBE {
	class Scene;

	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	class SceneNode;

	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		RasterizationPipelineInstance *pipeline_instance = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
		bool active = true;
		bool use_transform_matrix_as_push_constant = true;
	};

	class MeshRendererSystem : public System {

	public:
		MeshRendererSystem(Scene *scene);

		~MeshRendererSystem();

		void onPrepareRenderGraph(RenderGraph *render_graph);

	private :
		void onPrepareRenderGraphOrdered(RenderGraph *render_graph, SceneNode &node);
	};
}

