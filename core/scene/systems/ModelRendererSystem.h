#pragma once
#include "Core.h"
#include "HBETypes.h"
#include "core/scene/System.h"
#include "dependencies/utils-collection/Profiler.h"
#include "core/graphics/Graphics.h"
#include "core/resource/Model.h"
#include "core/scene/components/ModelRenderer.h"
namespace HBE {
	class ModelRendererSystem : public System {
		event_subscription_id on_draw_subcription_id;
	public:
		ModelRendererSystem(Scene* scene);
		~ModelRendererSystem();
		void draw(RenderGraph* render_graph);


	};

}
