
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
#ifdef USE_ENTT
		for (auto entity_handle:group) {
			Transform &transform = group.get<Transform>(entity_handle);
			Rotator &rotator = group.get<Rotator>(entity_handle);

			transform.rotate(rotator.rotate_speed * delta_t * rotator.angle);
		}
#else
		Rotator *rotators = scene->get<Rotator>();
		Transform *transforms = scene->get<Transform>();

		if (paralelize) {
			std::vector<std::thread> threads;
			uint32_t nb_thread = 6;
			uint32_t thread_offset = (group.size() - (group.size() % nb_thread)) / nb_thread;
			for (uint32_t i = 0; i < nb_thread; ++i) {
				uint32_t position = i * thread_offset;
				threads.emplace_back(process,
									 delta_t,
									 position,
									 i == nb_thread-1 ? group.size() : position + thread_offset,
									 transforms,
									 rotators);
			}
			for (size_t i = 0; i < nb_thread; ++i) {
				threads[i].join();
			}
		} else {
			process(delta_t, 0, group.size(), transforms, rotators);
		}
#endif
		Profiler::end();
	}
}

