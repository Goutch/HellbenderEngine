#pragma once

#include "core/scene/System.h"

namespace HBE {
	class CameraControllerSystem : public System {
	public:

		CameraControllerSystem(Scene *scene);
		~CameraControllerSystem();
		void update(float delta_t);

	};
}

