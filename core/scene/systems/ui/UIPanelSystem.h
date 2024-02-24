#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/Entity.h"

using namespace HBE;
namespace HBE {
	class Mesh;

	class GraphicPipelineInstance;

	class RenderGraph;

	class SceneNode;

	struct UIPanel {
		GraphicPipelineInstance *pipeline_instance = nullptr;
		PIVOT anchor = PIVOT_CENTER;
		int layer = 0;
		bool active = true;
		vec2 size = vec2(1, 1);
	};


	class HB_API UIPanelSystem : public System {
		struct PanelPushConstant {
			mat4 world_matrix;
			vec2 size;
		};
	public:
		void onDrawSceneNode(RenderGraph *graph, SceneNode &node);

		UIPanelSystem(Scene *scene);

		~UIPanelSystem();

	private:
		Mesh *anchor_meshes[9];


	};


}