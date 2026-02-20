#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/ecs/Component.h"
#include "dependencies/utils-collection/Event.h"

namespace HBE
{
	struct PushConstantInfo;

	class Scene;

	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	class MeshRendererSystem : public System
	{
		event_subscription_id on_draw_subscription_id;
	public:
		MeshRendererSystem(Scene* scene);

		~MeshRendererSystem();

		void onDraw(RenderGraph* render_graph);
	};
}
