#pragma once

#include "core/scene/System.h"
#include "core/scene/Scene.h"

namespace HBE {
	class CameraControllerSystem : public System {
	public:
		CameraControllerSystem(Scene *scene);
		~CameraControllerSystem();
		void onDetach(Entity entity);
		void update(float delta_t);
	};
}

