#pragma once

#include <core/scene/System.h>
#include <core/scene/Scene.h>
#include "Core.h"

namespace HBE {

	class HB_API UIRendererSystem : public System {

	public:
		UIRendererSystem(Scene *scene);
		~UIRendererSystem();
		void draw();
	};
}
