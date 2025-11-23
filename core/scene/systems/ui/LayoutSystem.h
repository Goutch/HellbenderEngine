#pragma once
#include <vector>
#include <core/scene/Scene.h>

#include "HBETypes.h"
#include "core/scene/System.h"
#include "core/scene/ecs/ComponentPool.h"

namespace HBE
{
	struct HB_API LayoutElement
	{
		entity_handle root;
		PIVOT pivot = PIVOT_CENTER;
		float width = 0;
		float height = 0;
	};

	class LayoutSystem : public HBE::System
	{
	public:
		LayoutSystem(Scene* scene);
		void update(float delta);
		void updateSizes(Node3D* node);
		~LayoutSystem();
	};
}
