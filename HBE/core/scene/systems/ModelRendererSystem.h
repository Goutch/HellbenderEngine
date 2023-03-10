#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/scene/Components.h"
#include "core/scene/System.h"
namespace HBE {
	class Scene;

	class HB_API ModelRendererSystem : public System {
	public:
		ModelRendererSystem(Scene* scene);
		~ModelRendererSystem();
		void draw(RenderGraph* render_graph);
	};

}
