#pragma once

#include "Core.h"
#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "Event.h"

namespace HBE {
	class Scene;

	class RenderGraph;

	class CameraSystem : public System {
		event_subscription_id on_render_subscription_id;
		event_subscription_id on_attach_3D_subscription_id;
		event_subscription_id on_attach_2D_subscription_id;
		event_subscription_id on_attach_pixel_subscription_id;
	public:
		CameraSystem(Scene *scene);

		~CameraSystem();

		void render(RenderGraph *render_graph);

		void onCameraAttached(Entity entity);

		void onCamera2DAttached(Entity entity);

		void onPixelCameraAttached(Entity entity);

	};
}


