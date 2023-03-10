#pragma once


#include "Core.h"
#include "core/scene/System.h"

namespace HBE {
	class Entity;

    class RenderGraph;
	class HB_API CameraSystem : public System {
		Scene *scene;
	public:
		CameraSystem(Scene *scene);

		~CameraSystem();

		void render(RenderGraph* render_graph);

		void onCameraAttached(Entity entity);

		void onCamera2DAttached(Entity entity);

		void onPixelCameraAttached(Entity entity);
	};

}


