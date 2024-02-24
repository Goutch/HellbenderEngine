
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"

namespace HBE {
	class Scene;

	class Mesh;

	class GraphicPipelineInstance;

	class RenderGraph;

	class SceneNode;

	struct HB_API MeshRenderer {
		Mesh *mesh = nullptr;
		GraphicPipelineInstance *pipeline_instance = nullptr;
		uint32_t layer = 0;
		bool ordered = false;
		bool active = true;
	};

	class MeshRendererSystem : public System {

	public:
		MeshRendererSystem(Scene *scene);

		~MeshRendererSystem();

		void draw(RenderGraph *render_graph);

	private :
		void drawSceneNode(RenderGraph *render_graph, SceneNode &node);

		void drawNode(SceneNode &node, RenderGraph *render_graph);
	};
}

