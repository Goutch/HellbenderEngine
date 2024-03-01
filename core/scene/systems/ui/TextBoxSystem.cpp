#include "TextBoxSystem.h"
#include "ButtonSystem.h"
#include "core/scene/Scene.h"
#include "core/input/Input.h"
namespace HBE {

	void TextBoxSystem::onUpdate(float delta) {
		if (active_textbox_entity.valid()) {
			TextBoxComponent &text_box_component = active_textbox_entity.get<TextBoxComponent>();
			ButtonComponent &button_component = text_box_component.button_entity.get<ButtonComponent>();
			cursor_blink_t += delta;
			if(cursor_blink_t > 1.0f) {
				cursor_blink_t = 0;
			}

			if(Input::getKeyDown(KEY_BACKSPACE)) {
				std::string current_text = button_component.getCurerntText();
				if(current_text.size() > 0) {
					current_text.pop_back();
					button_component.setText(current_text, nullptr);
				}
			}
			if(Input::getKeyDown(KEY_ENTER)) {
				active_textbox_entity = Entity::NULL_ENTITY;
			}
		}
	}

	TextBoxSystem::TextBoxSystem(Scene *scene) : System(scene) {
		scene->onAttach<TextBoxSystem>().subscribe(this, &TextBoxSystem::onAttachTextBox);
		scene->onUpdate.subscribe(this, &TextBoxSystem::onUpdate);
		Input::onCharDown.subscribe(this, &TextBoxSystem::onCharacterPressed);
	}

	void TextBoxSystem::onCharacterPressed(char character) {
		if (active_textbox_entity.valid()) {
			TextBoxComponent &text_box_component = active_textbox_entity.get<TextBoxComponent>();
			ButtonComponent &button_component = text_box_component.button_entity.get<ButtonComponent>();
			std::string current_text = button_component.getCurerntText();
			current_text += (char)character;
			button_component.setText(current_text, nullptr);
		}
	}

	void TextBoxSystem::onAttachTextBox(Entity text_box) {
		TextBoxComponent &text_box_component = text_box.get<TextBoxComponent>();
		text_box_component.button_entity = scene->createEntity3D();

		ButtonComponent &button_component = text_box_component.button_entity.attach<ButtonComponent>();

		button_component.onButtonClicked.subscribe(this, &TextBoxSystem::onTextBoxClicked);
	}

	void TextBoxSystem::onTextBoxClicked(Entity button) {
		active_textbox_entity = button;
		cursor_blink_t = 0;
	}
}