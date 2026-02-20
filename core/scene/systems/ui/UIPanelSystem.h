#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "core/scene/ecs/Component.h"
#include "Event.h"

using namespace HBE;
namespace HBE {
	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	struct Node;


	struct UIPanel {
		COMPONENT_IDS(UIPanel)
		RasterizationPipelineInstance *pipeline_instance = nullptr;
		PIVOT anchor = PIVOT_CENTER;
		int layer = 0;
		bool active = true;
		vec2 size = vec2(1, 1);
	};


	/**
	 * Draw anchored quads
	 */
	class HB_API UIPanelSystem : public System {
		struct PanelPushConstant {
			mat4 world_matrix;
			vec2 size;
		};
		event_subscription_id on_draw_subscription_id;
	public:
		UIPanelSystem(Scene *scene);

		~UIPanelSystem();

		void draw(RenderGraph *graph);

	private:
		Mesh *anchor_meshes[9];

	};


}