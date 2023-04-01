#pragma once

#include "Core.h"

namespace HBE {
	class Scene;
	class HB_API System {
	protected:
		Scene *scene;
	public:
		virtual ~System() = default;

		System(Scene *scene);
	};
}