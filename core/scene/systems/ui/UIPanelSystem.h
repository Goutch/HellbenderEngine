#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/Entity.h"

using namespace HBE;
namespace HBE {
	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	class HierarchyNode;

	struct UIPanel {
		RasterizationPipelineInstance *pipeline_instance = nullptr;
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
		UIPanelSystem(Scene *scene);

		~UIPanelSystem();
		void draw(RenderGraph* graph);

	private:
		Mesh *anchor_meshes[9];


	};


}