#pragma once
#include "Core.h"
#include "Event.h"
#include "core/scene/System.h"
#include "core/resource/Model.h"

namespace HBE {
	class RenderGraph;

	class HB_API ModelRendererSystem : public System {
		event_subscription_id on_draw_subcription_id;
	public:
		ModelRendererSystem(Scene* scene);
		~ModelRendererSystem();
		void draw(RenderGraph* render_graph);
		void drawNode(RenderGraph *render_graph, HBE::Model &model, const HBE::ModelNode &node, mat4 parent_transform);
	};

}
