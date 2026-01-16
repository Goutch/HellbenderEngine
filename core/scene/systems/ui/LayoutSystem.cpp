//
// Created by username on 3/6/2025.
//

#include "LayoutSystem.h"

#include <core/scene/Scene.h>

namespace HBE
{
	LayoutSystem::LayoutSystem(HBE::Scene* scene): System(scene)
	{
		scene->onUpdate.subscribe(this, &LayoutSystem::update);
	}

	void LayoutSystem::update(float delta)
	{
		//update Sizes
		//update Positions
	}

	void LayoutSystem::updateSizes(Node* node)
	{
	}

	LayoutSystem::~LayoutSystem()
	{
		scene->onUpdate.unsubscribe(this);
	}
}
