
#pragma once

#include "core/scene/System.h"
#include "core/scene/Entity.h"
#include "Core.h"

using namespace HBE;
namespace HBE {
	struct HB_API TextBoxComponent {
		Entity button_entity;
	};

	class HB_API TextBoxSystem : public System {
	private:
		float cursor_blink_t = 0;
		Entity active_textbox_entity;
	public:
		TextBoxSystem(Scene *scene);
		void onAttachTextBox(Entity text_box);
		void onTextBoxClicked(Entity button);

		void onUpdate(float delta);

		void onCharacterPressed(char character);
	};

}
