#pragma once

#include <core/scene/System.h>
#include <core/scene/Scene.h>
#include "Core.h"

namespace HBE {

	class HB_API TextRendererSystem : public System {

	public:
		TextRendererSystem(Scene *scene);
		~TextRendererSystem();
		void draw();
		void onDetachTextRenderer(Entity entity);
	};
}
