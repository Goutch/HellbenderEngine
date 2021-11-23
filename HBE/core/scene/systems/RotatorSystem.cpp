
#include <core/input/Input.h>
#include "RotatorSystem.h"
#include "core/scene/Scene.h"
#include "core/utility/Profiler.h"
#include "thread"

namespace HBE {
	RotatorSystem::RotatorSystem(Scene *scene) : System(scene) {
		scene->onUpdate.subscribe(this, &RotatorSystem::update);
	}


	void process(float delta_t, uint32_t from, uint32_t to, Transform *transforms, Rotator *rotators) {
		for (size_t i = from; i < to; ++i) {
			transforms[i].rotate(rotators[i].rotate_speed * delta_t * rotators[i].angle);
		}
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

