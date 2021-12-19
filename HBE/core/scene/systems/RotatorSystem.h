#pragma once

#include <core/scene/Components.h>
#include "core/scene/System.h"
namespace HBE {
	struct HB_API Rotator {
		float rotate_speed = 1.0f;
		vec3 angle = vec3(0.0f, 0.0f, 0.0f);
	};

	class HB_API RotatorSystem : public System {
		bool paralelize = false;
	public:
		RotatorSystem(Scene *scene);
		void update(float delta_t);
	};

}

