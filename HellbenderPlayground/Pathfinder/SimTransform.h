#pragma once

#include "HBE.h"

using namespace HBE;
struct SimTransform {
	fixed3<f32> position;
};

class SimTransformSystem : public System {
public:

	SimTransformSystem(Scene *scene) : System(scene) {

		scene->onUpdate.subscribe(this, &SimTransformSystem::onUpdate);
	}

	~SimTransformSystem() {
		scene->onUpdate.unsubscribe(this);
	}

	void onUpdate(float delta) {
		auto group = scene->group<Transform, SimTransform>();
		for (auto[e, transform, simTransform]:group) {
			transform.setPosition(simTransform.position.toVec3());
		}
	}
};