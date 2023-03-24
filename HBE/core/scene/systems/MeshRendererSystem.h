
#pragma once

#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"

namespace HBE {
	class Scene;

	class RenderGraph;

	class SceneNode;

	class MeshRendererSystem : public System {

	public:
		MeshRendererSystem(Scene *scene);

		~MeshRendererSystem();

		void draw(RenderGraph *render_graph);

	private :
		void drawNode(SceneNode &node, RenderGraph *render_graph);

	};
}

