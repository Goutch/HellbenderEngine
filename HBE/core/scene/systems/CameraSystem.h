#pragma once


#include "Core.h"
#include "core/scene/System.h"

namespace HBE {
	class Entity;

	class HB_API CameraSystem : public System {
		Scene *scene;

	public:
		CameraSystem(Scene *scene);
		~CameraSystem();
		void render();
		void onCameraAttached(Entity entity);

		void onCamera2DAttached(Entity entity);
		void onCameraUIAttached(Entity entity);
	};

}


