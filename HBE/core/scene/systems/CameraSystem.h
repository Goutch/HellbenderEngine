#pragma once

#include "Core.h"
#include "core/scene/System.h"
#include "core/scene/Entity.h"


namespace HBE {
	class Scene;

	class RenderGraph;

	class CameraSystem : public System {
	public:
		CameraSystem(Scene *scene);

		~CameraSystem();

		void render(RenderGraph *render_graph);

		void onCameraAttached(Entity entity);

		void onCamera2DAttached(Entity entity);

		void onPixelCameraAttached(Entity entity);
	};
}


