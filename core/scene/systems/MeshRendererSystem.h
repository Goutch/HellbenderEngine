#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/ecs/Component.h"

namespace HBE
{
	struct PushConstantInfo;
	class Scene;

	class Mesh;

	class RasterizationPipelineInstance;

	class RenderGraph;

	class MeshRendererSystem : public System
	{
	public:
		MeshRendererSystem(Scene* scene);

		~MeshRendererSystem();

		void onDraw(RenderGraph* render_graph);
	};
}
