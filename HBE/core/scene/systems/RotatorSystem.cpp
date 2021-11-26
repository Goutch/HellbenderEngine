
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

		Profiler::begin("RotatorUpdate");
		Profiler::begin("RotatorUpdateGroup");
		auto group = scene->group<Transform, Rotator>();

		Profiler::end();
		for (auto [handle,transform,rotator]:group) {
			transform.rotate(rotator.rotate_speed * delta_t * rotator.angle);
		}
		Profiler::end();
	}
}

