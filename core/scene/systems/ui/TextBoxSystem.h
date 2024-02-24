
#pragma once

#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "Core.h"

using namespace HBE;
namespace HBE {
	struct HB_API TextBoxComponent {
		Entity button_entity;
		Entity cursor_entity;
	};

	class HB_API TextBoxSystem : public System {
	public:
		TextBoxSystem(Scene *scene);
	};

}
