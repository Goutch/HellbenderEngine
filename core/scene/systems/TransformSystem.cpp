//
// Created by username on 3/7/2025.
//

#include "TransformSystem.h"

#include <core/scene/Scene.h>

namespace HBE {
	void TransformSystem::onAttachTransform(Entity entity) {
		entity.get<Transform>()->entity = entity;
	}

	TransformSystem::TransformSystem(Scene *scene) : System(scene) {
		scene->onAttach<Transform>().subscribe(on_attach_transform_subscription_id, this, &TransformSystem::onAttachTransform);
	}
}
