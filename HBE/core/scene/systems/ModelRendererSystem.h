#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/utility/Profiler.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Model.h"
#include "core/scene/components/ModelRenderer.h"
namespace HBE {
	class ModelRendererSystem : public System {
	public:
		ModelRendererSystem(Scene* scene);
		~ModelRendererSystem();
		void draw(RenderGraph* render_graph);
	};

}
