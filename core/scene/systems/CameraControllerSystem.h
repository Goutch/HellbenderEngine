#pragma once

#include "core/scene/System.h"
#include "core/scene/Scene.h"

namespace HBE {
	class CameraControllerSystem : public System {
		event_subscription_id update_subscription_id;
		event_subscription_id detach_subscription_id;
		event_subscription_id attach_subscription_id;
	public:
		CameraControllerSystem(Scene *scene);

		~CameraControllerSystem();

		void onAttach(Entity entity);

		void onDetach(Entity entity);

		void update(float delta_t);

	};
}

