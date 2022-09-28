#pragma once

#include "core/scene/System.h"

namespace HBE {
	class Entity;
	class CameraControllerSystem : public System {
	public:

		CameraControllerSystem(Scene *scene);
		~CameraControllerSystem();
		void onDetach(Entity entity);
		void update(float delta_t);
	};
}

