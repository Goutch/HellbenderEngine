#pragma once
#include "core/scene/Entity.h"
#include "core/scene/System.h"

namespace HBE
{
	class TransformSystem : public System
	{
		void onAttachTransform(Entity entity);

	public:
		TransformSystem(Scene* scene);
	};
}
