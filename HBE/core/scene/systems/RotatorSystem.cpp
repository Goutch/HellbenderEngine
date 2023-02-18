
#include <core/input/Input.h>
#include "RotatorSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"

namespace HBE {
	RotatorSystem::RotatorSystem(Scene *scene) : System(scene) {
		scene->onUpdate.subscribe(this, &RotatorSystem::update);
	}

	void RotatorSystem::update(float delta_t) {
		if (Input::getKeyDown(KEY::P)) {
			paralelize = !paralelize;
		}

		HB_PROFILE_BEGIN("RotatorUpdate");
		HB_PROFILE_BEGIN("RotatorUpdateGroup");
		auto group = scene->group<Transform, Rotator>();

		HB_PROFILE_END();
		for (auto [handle,transform,rotator]:group) {
			transform.rotate(rotator.rotate_speed * delta_t * rotator.angle);
		}
		HB_PROFILE_END();
	}
}

