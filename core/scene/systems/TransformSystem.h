#pragma once
#include "core/scene/Entity.h"
#include "core/scene/System.h"
#include "Event.h"
namespace HBE
{
	class TransformSystem : public System
	{
		void onAttachTransform(Entity entity);

	public:
		TransformSystem(Scene* scene);

		event_subscription_id on_attach_transform_subscription_id;
	};
}
